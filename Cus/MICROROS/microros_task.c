#include "microros_task.h"
#include "gpio.h"
#include "usart.h"
#include "tim.h"
#include "motor.h"

#include <rcl/error_handling.h>
#include <rcl/rcl.h>
#include <rclc/executor.h>
#include <rclc/rclc.h>
#include <rmw_microros/rmw_microros.h>
#include <rmw_microxrcedds_c/config.h>
#include <uxr/client/transport.h>
#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/float64.h>
#include <geometry_msgs/msg/twist.h>
#include <math.h>

/* Robot parameters */
#define WHEEL_SEPARATION 0.32f
#define WHEEL_RADIUS 0.065f
#define MAX_LINEAR_SPEED 1.0f
#define MAX_ANGULAR_SPEED 2.0f
#define RPM_TO_RADS 0.10472f
#define RADS_TO_RPM 9.5493f

/* Static variables */
static rcl_publisher_t publisher;
static std_msgs__msg__Int32 msg;
static rclc_support_t support;
static rcl_allocator_t allocator;
static rclc_executor_t executor;
static rcl_node_t node;
static rcl_timer_t timer;
static int countt;
static rcl_ret_t temp_ret;

/* Subscribers */
static rcl_subscription_t motor1_subscriber;
static rcl_subscription_t motor2_subscriber;
static rcl_subscription_t cmd_vel_subscriber;
static std_msgs__msg__Float64 motor1_msg;
static std_msgs__msg__Float64 motor2_msg;
static geometry_msgs__msg__Twist cmd_vel_msg;

/* Static function declarations */
static void timer_callback(rcl_timer_t *timer, int64_t last_call_time);
static void motor1_callback(const void * msgin);
static void motor2_callback(const void * msgin);
static void cmd_vel_callback(const void * msgin);
static void calculate_wheel_speeds(float linear_x, float angular_z, float *left_rpm, float *right_rpm);


static rcl_publisher_t right_wheel_feedback_publisher;
static rcl_publisher_t left_wheel_feedback_publisher;
static rcl_publisher_t right_wheel_target_publisher;
static rcl_publisher_t left_wheel_target_publisher;

static std_msgs__msg__Float64 right_wheel_feedback_msg;
static std_msgs__msg__Float64 left_wheel_feedback_msg;
static std_msgs__msg__Float64 right_wheel_target_msg;
static std_msgs__msg__Float64 left_wheel_target_msg;

/* Timer callback implementation */
static void timer_callback(rcl_timer_t *timer, int64_t last_call_time) {
    if (timer != NULL) {
        msg.data = countt--;
        temp_ret = rcl_publish(&publisher, &msg, NULL);

        // 更新并发布右轮状态
        // right_wheel_feedback_msg.data = (int32_t)hmotor1.current_rpm;  // 实际速度
        // right_wheel_target_msg.data = (int32_t)hmotor1.target_rpm;     // 目标速度

        right_wheel_feedback_msg.data = hmotor1.current_rpm;  // 实际速度
        right_wheel_target_msg.data = hmotor1.target_rpm;     // 目标速度
        rcl_publish(&right_wheel_feedback_publisher, &right_wheel_feedback_msg, NULL);
        rcl_publish(&right_wheel_target_publisher, &right_wheel_target_msg, NULL);
        
        // 更新并发布左轮状态
        left_wheel_feedback_msg.data = hmotor2.current_rpm;   // 实际速度
        left_wheel_target_msg.data = hmotor2.target_rpm;      // 目标速度
        rcl_publish(&left_wheel_feedback_publisher, &left_wheel_feedback_msg, NULL);
        rcl_publish(&left_wheel_target_publisher, &left_wheel_target_msg, NULL);
    }
}

/* Motor callbacks implementation */
static void motor1_callback(const void * msgin) {
    const std_msgs__msg__Float64 * msg = (const std_msgs__msg__Float64 *)msgin;
    float target_rpm = (float)msg->data;
    Motor_SetTargetSpeed(&hmotor1, target_rpm);
}

static void motor2_callback(const void * msgin) {
    const std_msgs__msg__Float64 * msg = (const std_msgs__msg__Float64 *)msgin;
    float target_rpm = (float)msg->data;
    Motor_SetTargetSpeed(&hmotor2, target_rpm);
}

/* Cmd_vel callback implementation */
static void cmd_vel_callback(const void * msgin) {
    const geometry_msgs__msg__Twist * msg = (const geometry_msgs__msg__Twist *)msgin;
    float linear_x = (float)msg->linear.x;
    float angular_z = (float)msg->angular.z;
    
    // Limit speed range
    linear_x = fminf(fmaxf(linear_x, -MAX_LINEAR_SPEED), MAX_LINEAR_SPEED);
    angular_z = fminf(fmaxf(angular_z, -MAX_ANGULAR_SPEED), MAX_ANGULAR_SPEED);
    
    float left_rpm, right_rpm;
    calculate_wheel_speeds(linear_x, angular_z, &left_rpm, &right_rpm);
    
    Motor_SetTargetSpeed(&hmotor1, left_rpm);
    Motor_SetTargetSpeed(&hmotor2, right_rpm);
}

/* Wheel speed calculation */
static void calculate_wheel_speeds(float linear_x, float angular_z, float *left_rpm, float *right_rpm) {
    float left_wheel_speed = (linear_x - angular_z * WHEEL_SEPARATION / 2.0f) / WHEEL_RADIUS;
    float right_wheel_speed = (linear_x + angular_z * WHEEL_SEPARATION / 2.0f) / WHEEL_RADIUS;
    
    *left_rpm = left_wheel_speed * RADS_TO_RPM;
    *right_rpm = right_wheel_speed * RADS_TO_RPM;
}

/* MicroROS initialization */
void MicroROS_Init(void) {
    rmw_uros_set_custom_transport(
        true,
        (void *)&huart1,
        cubemx_transport_open,
        cubemx_transport_close,
        cubemx_transport_write,
        cubemx_transport_read
    );

    rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
    freeRTOS_allocator.allocate = microros_allocate;
    freeRTOS_allocator.deallocate = microros_deallocate;
    freeRTOS_allocator.reallocate = microros_reallocate;
    freeRTOS_allocator.zero_allocate = microros_zero_allocate;

    if (!rcutils_set_default_allocator(&freeRTOS_allocator)) {
        printf("Error on default allocators (line %d)\n", __LINE__);
    }

    allocator = rcl_get_default_allocator();
    rclc_support_init(&support, 0, NULL, &allocator);
    rclc_node_init_default(&node, "cubemx_node", "", &support);
    rclc_executor_init(&executor, &support.context, 12, &allocator);

    // Initialize publisher
    rclc_publisher_init_default(
        &publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "ping_ping"
    );

    // 初始化右轮速度发布者
    rclc_publisher_init_default(
        &right_wheel_feedback_publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
        "wheel_right/feedback"
    );
    
    rclc_publisher_init_default(
        &right_wheel_target_publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
        "wheel_right/target"
    );
    
    // 初始化左轮速度发布者
    rclc_publisher_init_default(
        &left_wheel_feedback_publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
        "wheel_left/feedback"
    );
    
    rclc_publisher_init_default(
        &left_wheel_target_publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
        "wheel_left/target"
    );

    // Initialize subscribers
    rclc_subscription_init_default(
        &motor1_subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
        "wheel_right/target_speed"
    );

    rclc_subscription_init_default(
        &motor2_subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
        "wheel_left/target_speed"
    );

    rclc_subscription_init_default(
        &cmd_vel_subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        "cmd_vel"
    );

    // Initialize timer
    rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(10), timer_callback);

    // Add timer and subscribers to executor
    rclc_executor_add_timer(&executor, &timer);
    rclc_executor_add_subscription(&executor, &motor1_subscriber, &motor1_msg, &motor1_callback, ON_NEW_DATA);
    rclc_executor_add_subscription(&executor, &motor2_subscriber, &motor2_msg, &motor2_callback, ON_NEW_DATA);
    rclc_executor_add_subscription(&executor, &cmd_vel_subscriber, &cmd_vel_msg, &cmd_vel_callback, ON_NEW_DATA);

    // Initialize motors and encoders
    Motors_Init();
    Encoders_Init();
}

/* MicroROS task implementation */
void MicroROS_TaskStart(void *argument) {
    for (;;) {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));

        msg.data++;
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
        osDelay(200);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
        osDelay(200);
    }
}