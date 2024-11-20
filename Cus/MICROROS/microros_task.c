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
#define WHEEL_SEPARATION 0.2f  //m
#define WHEEL_RADIUS 0.02f  //m
#define MAX_LINEAR_SPEED 5.0f  //CMD_VEL
#define MAX_ANGULAR_SPEED 180.0f //CMD_VEL
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
static std_msgs__msg__String pid_params_msg = {0};

/* Global parameters */
// // static VehicleParams current_vehicle_params = {
// VehicleParams current_vehicle_params = {
//     .type = VEHICLE_TYPE_DIFFERENTIAL,  // 默认为差速
//     .wheelRadius = 0.065f,
//     .vehicleWidth = 0.32f,
//     .vehicleLength = 0.32f
// };

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
// static void timer_callback(rcl_timer_t *timer, int64_t last_call_time) {
//     if (timer != NULL) {
//         msg.data = countt--;
//         temp_ret = rcl_publish(&publisher, &msg, NULL);

//         // 更新并发布右轮状态
//         // right_wheel_feedback_msg.data = (int32_t)hmotor1.current_rpm;  // 实际速度
//         // right_wheel_target_msg.data = (int32_t)hmotor1.target_rpm;     // 目标速度

//         right_wheel_feedback_msg.data = hmotor1.current_rpm;  // 实际速度
//         right_wheel_target_msg.data = hmotor1.target_rpm;     // 目标速度
//         rcl_publish(&right_wheel_feedback_publisher, &right_wheel_feedback_msg, NULL);
//         // rcl_publish(&right_wheel_target_publisher, &right_wheel_target_msg, NULL);
        
//         // 更新并发布左轮状态
//         left_wheel_feedback_msg.data = hmotor2.current_rpm;   // 实际速度
//         left_wheel_target_msg.data = hmotor2.target_rpm;      // 目标速度
//         rcl_publish(&left_wheel_feedback_publisher, &left_wheel_feedback_msg, NULL);
//         // rcl_publish(&left_wheel_target_publisher, &left_wheel_target_msg, NULL);
//     // }
// }

static void timer_callback(rcl_timer_t *timer, int64_t last_call_time) {
    if (timer != NULL) {
        msg.data = countt--;
        temp_ret = rcl_publish(&publisher, &msg, NULL);

        // 1. 先将当前RPM转换为弧度/秒
        float right_wheel_rads = hmotor1.current_rpm * 1;  // RPM -> rad/s
        float left_wheel_rads = hmotor2.current_rpm * 1;   // RPM -> rad/s

        // 2. 从弧度/秒转换为线速度 (m/s)
        float right_wheel_speed = right_wheel_rads * WHEEL_RADIUS;  // rad/s * r = m/s
        float left_wheel_speed = left_wheel_rads * WHEEL_RADIUS;    // rad/s * r = m/s

        // 3. 更新并发布轮子状态（单位：m/s）
        right_wheel_feedback_msg.data = right_wheel_speed;  // 右轮实际线速度
        left_wheel_feedback_msg.data = left_wheel_speed;    // 左轮实际线速度

        // 4. 发布消息
        rcl_publish(&right_wheel_feedback_publisher, &right_wheel_feedback_msg, NULL);
        rcl_publish(&left_wheel_feedback_publisher, &left_wheel_feedback_msg, NULL);
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
    
    Motor_SetTargetSpeed(&hmotor1, left_rpm *10);
    Motor_SetTargetSpeed(&hmotor2, right_rpm *10);
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
    const std_msgs__msg__String * msg = (const std_msgs__msg__String *)msgin;
    if (msg == NULL || msg->data.data == NULL) {
        return;
    }
    
    // 创建临时缓冲区来存储消息数据
    char buffer[100];
    strncpy(buffer, msg->data.data, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';  // 确保字符串结束
    
    // 移除可能存在的首尾单引号
    if (buffer[0] == '\'') {
        memmove(buffer, buffer + 1, strlen(buffer));
    }
    if (buffer[strlen(buffer) - 1] == '\'') {
        buffer[strlen(buffer) - 1] = '\0';
    }
    
    // 移除 JSON 的大括号
    char* json_start = strchr(buffer, '{');
    char* json_end = strrchr(buffer, '}');
    if (json_start && json_end) {
        *json_end = '\0';
        json_start++; // 跳过 {
    } else {
        return; // JSON 格式错误
    }
    
    // 初始化 PID 参数
    float p = 0.0f, i = 0.0f, d = 0.0f;
    
    // 使用 strtok 解析每个键值对
    char* token = strtok(json_start, ",");
    while (token != NULL) {
        // 去除首尾空格
        while (*token == ' ') token++;
        char* end = token + strlen(token) - 1;
        while (end > token && (*end == ' ' || *end == '"')) {
            *end = '\0';
            end--;
        }
        
        // 查找键值分隔符 ":"
        char* separator = strchr(token, ':');
        if (separator) {
            *separator = '\0';
            char* key = token;
            char* value = separator + 1;
            
            // 去除键中的引号
            if (key[0] == '"') key++;
            if (key[strlen(key) - 1] == '"') key[strlen(key) - 1] = '\0';
            
            // 根据键名设置相应的值
            if (strcmp(key, "p") == 0) {
                p = atof(value);
            } else if (strcmp(key, "i") == 0) {
                i = atof(value);
            } else if (strcmp(key, "d") == 0) {
                d = atof(value);
            }
        }
        
        token = strtok(NULL, ",");
    }
    
    // printf("Parsed PID values - P: %.3f, I: %.3f, D: %.3f\n", p, i, d);
    gggg++;
    // 更新 PID 参数
    if (p >= 0.0f) {
            gggg++;
        if (MicroROSState_UpdatePIDParams(&hmotor1.pid, p, i, d)) {
                gggg++;
            printf("Motor1 PID params updated successfully\n");
        }
        if (MicroROSState_UpdatePIDParams(&hmotor2.pid, p, i, d)) {
            printf("Motor2 PID params updated successfully\n");
        }
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

    // 创建临时缓冲区来存储消息数据
    char buffer[100];
    strncpy(buffer, msg->data.data, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';  // 确保字符串结束

    // 用于存储解析后的参数
    VehicleType parsed_type = VEHICLE_TYPE_DIFFERENTIAL;
    float wheel_radius = 0.0f;
    float vehicle_width = 0.0f;
    float vehicle_length = 0.0f;

    // 使用strtok解析字符串
    char* token = strtok(buffer, ",");
    int param_count = 0;
    
    while (token != NULL && param_count < 4) {
        // 去除首尾空格
        while (*token == ' ') token++;
        char* end = token + strlen(token) - 1;
        while (end > token && *end == ' ') {
            *end = '\0';
            end--;
        }

        switch (param_count) {
            case 0:  // 车辆类型
                if (strcmp(token, "differential") == 0) {
                    parsed_type = VEHICLE_TYPE_DIFFERENTIAL;
                } else if (strcmp(token, "ackermann") == 0) {
                    parsed_type = VEHICLE_TYPE_ACKERMANN;
                } else if (strcmp(token, "mecanum") == 0) {
                    parsed_type = VEHICLE_TYPE_MECANUM;
                } else if (strcmp(token, "boat") == 0) {
                    parsed_type = VEHICLE_TYPE_BOAT;
                }
                break;
                
            case 1:  // 轮半径
                wheel_radius = atof(token);
                break;
                
            case 2:  // 车身宽度
                vehicle_width = atof(token);
                break;
                
            case 3:  // 车身长度
                vehicle_length = atof(token);
                break;
        }
        
        token = strtok(NULL, ",");
        param_count++;
    }

    // 检查是否获得了所有参数
    if (param_count == 4) {
        // 使用状态管理器更新参数
        if (MicroROSState_SetVehicleParams(parsed_type, 
                                         wheel_radius, 
                                         vehicle_width, 
                                         vehicle_length)) {
            // 参数更新成功，获取更新后的参数
            const VehicleParams* params = MicroROSState_GetVehicleParams();
            
            // 打印确认信息（可选）
            // printf("Vehicle parameters updated:\n");
            // printf("Type: %s\n", MicroROSState_GetVehicleTypeString(params->type));
            // printf("Wheel Radius: %.3f m\n", params->wheelRadius);
            // printf("Vehicle Width: %.3f m\n", params->vehicleWidth);
            // printf("Vehicle Length: %.3f m\n", params->vehicleLength);
            
            // TODO: 如果需要，这里可以添加其他更新逻辑
            // 例如：更新运动学模型参数等
        } else {
            printf("Error: Invalid vehicle parameters\n");
        }
    } else {
        printf("Error: Invalid parameter count (expected 4, got %d)\n", param_count);
    }
}

/* MicroROS initialization */
void MicroROS_Init(void) {

    MicroROSState_Init(); 

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
        "wheel_right/target_speed"
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
        "wheel_left/target_speed"
    );

    // Initialize subscribers
    rclc_subscription_init_default(
        &motor1_subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
        "wheel_right/target"
    );

    rclc_subscription_init_default(
        &motor2_subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
        "wheel_left/target"
    );

    rclc_subscription_init_default(
        &cmd_vel_subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        "cmd_vel"
    );

    // 在创建订阅者之前，先初始化消息结构

    // 为String消息分配内存
    pid_params_msg.data.capacity = 100;  // 最大字符数
    pid_params_msg.data.size = 0;
    pid_params_msg.data.data = (char*)malloc(pid_params_msg.data.capacity);

    // 初始化订阅者
    rcl_ret_t ret = rclc_subscription_init_default(
        &pid_params_subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
        "pid_params"
    );

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
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));

        msg.data++;
        // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
        // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
        osDelay(10);
        // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
        // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
        // osDelay(200);
    }
}