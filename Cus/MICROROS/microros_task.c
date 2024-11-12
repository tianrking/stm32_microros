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

static rcl_subscription_t vehicle_params_subscriber;
static std_msgs__msg__String vehicle_params_msg;

static rcl_subscription_t pid_params_subscriber;
static std_msgs__msg__Float32MultiArray pid_params_msg = {0};  // 初始化为0

/* Global parameters */
// static VehicleParams current_vehicle_params = {
VehicleParams current_vehicle_params = {
    .type = VEHICLE_TYPE_DIFFERENTIAL,  // 默认为差速
    .wheelRadius = 0.065f,
    .vehicleWidth = 0.32f,
    .vehicleLength = 0.32f
};

// static PIDParams current_pid_params = {
PIDParams current_pid_params = {
    .p = 1.0f,
    .i = 0.0f,
    .d = 0.0f
};

/* Static function declarations */
static void timer_callback(rcl_timer_t *timer, int64_t last_call_time);
static void motor1_callback(const void * msgin);
static void motor2_callback(const void * msgin);
static void cmd_vel_callback(const void * msgin);
static void calculate_wheel_speeds(float linear_x, float angular_z, float *left_rpm, float *right_rpm);


static void pid_params_callback(const void * msgin);
static void vehicle_params_callback(const void * msgin);
static VehicleType string_to_vehicle_type(const char* type_str);


static rcl_publisher_t right_wheel_feedback_publisher;
static rcl_publisher_t left_wheel_feedback_publisher;
static rcl_publisher_t right_wheel_target_publisher;
static rcl_publisher_t left_wheel_target_publisher;

static std_msgs__msg__Float64 right_wheel_feedback_msg;
static std_msgs__msg__Float64 left_wheel_feedback_msg;
static std_msgs__msg__Float64 right_wheel_target_msg;
static std_msgs__msg__Float64 left_wheel_target_msg;

// 修改订阅者声明
static rcl_subscription_t vehicle_params_subscriber;
static std_msgs__msg__String vehicle_params_msg = {0};  // 初始化为0

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

float gggg;

/* Motor callbacks implementation */
static void motor1_callback(const void * msgin) {
    gggg++;
    const std_msgs__msg__Float64 * msg = (const std_msgs__msg__Float64 *)msgin;
    float target_rpm = (float)msg->data;
    Motor_SetTargetSpeed(&hmotor1, target_rpm);
}


static void motor2_callback(const void * msgin) {
    gggg++;
    const std_msgs__msg__Float64 * msg = (const std_msgs__msg__Float64 *)msgin;
    float target_rpm = (float)msg->data;
    Motor_SetTargetSpeed(&hmotor2, target_rpm);
}

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


/* PID parameters callback */
static void pid_params_callback(const void * msgin) {
    const std_msgs__msg__Float32MultiArray * msg = (const std_msgs__msg__Float32MultiArray *)msgin;
    
    if (msg == NULL) {
        return;
    }

    // 打印调试信息
    printf("Received PID params, size: %d\n", msg->data.size);
    
    // 确保收到了3个参数
    if (msg->data.size == 3) {
        current_pid_params.p = msg->data.data[0];
        current_pid_params.i = msg->data.data[1];
        current_pid_params.d = msg->data.data[2];
        
        // 打印接收到的参数
        // printf("New PID params: P=%.2f, I=%.2f, D=%.2f\n", 
        //        current_pid_params.p,
        //        current_pid_params.i,
        //        current_pid_params.d);
               
        gggg++;  // 更新计数器
        
        // 更新电机PID参数
        // Motor_UpdatePIDParams(&hmotor1, 
        //                     current_pid_params.p, 
        //                     current_pid_params.i, 
        //                     current_pid_params.d);
        // Motor_UpdatePIDParams(&hmotor2, 
        //                     current_pid_params.p, 
        //                     current_pid_params.i, 
        //                     current_pid_params.d);
    }
}

// 字符串解析辅助函数
static char* str_split_next(char* str, char delim, char** next) {
    char* token = str;
    if (str == NULL) return NULL;
    
    *next = strchr(str, delim);
    if (*next != NULL) {
        **next = '\0';  // 将分隔符替换为字符串结束符
        *next = *next + 1;  // 移动到下一个字符
    }
    return token;
}

static void vehicle_params_callback(const void * msgin) {
    const std_msgs__msg__String * msg = (const std_msgs__msg__String *)msgin;
    if (msg == NULL || msg->data.data == NULL) {
        return;
    }

    // 创建一个临时缓冲区来存储消息数据，因为strtok会修改原字符串
    char buffer[100];
    strncpy(buffer, msg->data.data, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';  // 确保字符串结束

    char *next_token = NULL;
    char *token;

    // 解析车辆类型
    token = str_split_next(buffer, ',', &next_token);
    if (token != NULL) {
        // 设置车辆类型
        if (strcmp(token, "differential") == 0) {
            current_vehicle_params.type = VEHICLE_TYPE_DIFFERENTIAL;
        } else if (strcmp(token, "ackermann") == 0) {
            current_vehicle_params.type = VEHICLE_TYPE_ACKERMANN;
        } else if (strcmp(token, "mecanum") == 0) {
            current_vehicle_params.type = VEHICLE_TYPE_MECANUM;
        } else if (strcmp(token, "boat") == 0) {
            current_vehicle_params.type = VEHICLE_TYPE_BOAT;
        }

        // 解析轮半径
        token = str_split_next(next_token, ',', &next_token);
        if (token != NULL) {
            current_vehicle_params.wheelRadius = atof(token);

            // 解析车宽
            token = str_split_next(next_token, ',', &next_token);
            if (token != NULL) {
                current_vehicle_params.vehicleWidth = atof(token);

                // 解析车长
                token = str_split_next(next_token, ',', &next_token);
                if (token != NULL) {
                    current_vehicle_params.vehicleLength = atof(token);

                    // 打印接收到的参数
                    // printf("Vehicle Params updated:\n");
                    // printf("Type: %d\n", current_vehicle_params.type);
                    // printf("Wheel Radius: %.3f\n", current_vehicle_params.wheelRadius);
                    // printf("Width: %.3f\n", current_vehicle_params.vehicleWidth);
                    // printf("Length: %.3f\n", current_vehicle_params.vehicleLength);
                }
            }
        }
    }
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
    rclc_executor_init(&executor, &support.context, 16, &allocator);

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

    // 在创建订阅者之前，先初始化消息结构
    pid_params_msg.data.capacity = 3;
    pid_params_msg.data.size = 0;
    pid_params_msg.data.data = (float*)malloc(3 * sizeof(float));

    // 初始化PID参数订阅者
    rcl_ret_t ret = rclc_subscription_init_default(
        &pid_params_subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32MultiArray),
        "pid_params"
    );
    
    if (ret != RCL_RET_OK) {
        // printf("Error creating pid_params subscriber: %d\n", ret);
    }

    // 为String消息分配内存
    vehicle_params_msg.data.capacity = 100;  // 最大字符数
    vehicle_params_msg.data.size = 0;
    vehicle_params_msg.data.data = (char*)malloc(vehicle_params_msg.data.capacity);

    ret = rclc_subscription_init_default(
        &vehicle_params_subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
        "vehicle_params"
    );


    // Initialize timer
    rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(10), timer_callback);

    // Add timer and subscribers to executor
    rclc_executor_add_timer(&executor, &timer);
    rclc_executor_add_subscription(&executor, &motor1_subscriber, &motor1_msg, &motor1_callback, ON_NEW_DATA);
    rclc_executor_add_subscription(&executor, &motor2_subscriber, &motor2_msg, &motor2_callback, ON_NEW_DATA);
    rclc_executor_add_subscription(&executor, &cmd_vel_subscriber, &cmd_vel_msg, &cmd_vel_callback, ON_NEW_DATA);

    ret = rclc_executor_add_subscription(
        &executor,
        &pid_params_subscriber,
        &pid_params_msg,
        &pid_params_callback,
        ON_NEW_DATA
    );
    rclc_executor_add_subscription(&executor, &vehicle_params_subscriber, &vehicle_params_msg, &vehicle_params_callback, ON_NEW_DATA);


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