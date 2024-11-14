// #include "microros_task.h"
// #include "gpio.h"
// #include "usart.h"
// #include "tim.h"
// #include "motor.h"

// #include <rcl/error_handling.h>
// #include <rcl/rcl.h>
// #include <rclc/executor.h>
// #include <rclc/rclc.h>
// #include <rmw_microros/rmw_microros.h>
// #include <rmw_microxrcedds_c/config.h>
// #include <uxr/client/transport.h>
// #include <std_msgs/msg/int32.h>
// #include <std_msgs/msg/float64.h>
// #include <geometry_msgs/msg/twist.h>
// #include <math.h>
// #include "cmsis_os.h"

// /* Constants */
// #define MAX_QUEUED_MESSAGES 10
// #define MAX_PROCESS_PER_SPIN 5  // 每次spin最多处理的消息数
// #define EXECUTOR_RESET_INTERVAL 5000  // 每5秒检查一次执行器状态
// #define QUEUE_WARNING_THRESHOLD 8     // 队列警告阈值

// /* Wheel parameters */
// #define WHEEL_SEPARATION 0.32f
// #define WHEEL_RADIUS 0.065f
// #define MAX_LINEAR_SPEED 1.0f
// #define MAX_ANGULAR_SPEED 2.0f
// #define RPM_TO_RADS 0.10472f
// #define RADS_TO_RPM 9.5493f

// /* Monitoring counters */
// static volatile uint32_t total_messages = 0;     // 总接收消息数
// static volatile uint32_t processed_messages = 0;  // 已处理消息数
// static volatile uint32_t dropped_messages = 0;    // 丢弃的消息数
// static volatile uint32_t current_queue = 0;      // 当前队列大小
// static volatile uint32_t max_queue = 0;          // 最大队列大小
// static volatile uint32_t last_executor_run = 0;  // 最后执行器运行时间
// static volatile uint32_t executor_errors = 0;    // 执行器错误次数
// static volatile uint32_t publish_errors = 0;     // 发布错误次数

// /* Status flags */
// static volatile bool executor_healthy = true;
// static volatile bool queue_warning = false;

// /* Function declarations */
// static void timer_callback(rcl_timer_t *timer, int64_t last_call_time);
// static void motor1_callback(const void * msgin);
// static void motor2_callback(const void * msgin);
// static void cmd_vel_callback(const void * msgin);
// static void calculate_wheel_speeds(float linear_x, float angular_z, float *left_rpm, float *right_rpm);
// static void check_executor_health(void);
// static void reset_executors_if_needed(void);
// void debug_print(const char* str);
// void num_to_str(uint32_t num, char* str);

// /* Debug functions */
// void debug_print(const char* str) {
//     HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), 100);
// }

// void num_to_str(uint32_t num, char* str) {
//     char temp[20];
//     int idx = 0;
//     do {
//         temp[idx++] = num % 10 + '0';
//         num /= 10;
//     } while(num > 0);
    
//     int j = 0;
//     while(idx > 0) {
//         str[j++] = temp[--idx];
//     }
//     str[j] = '\0';
// }

// /* Static variables */
// static rcl_publisher_t publisher;
// static std_msgs__msg__Int32 msg;
// static rclc_support_t support;
// static rcl_allocator_t allocator;
// static rclc_executor_t executor_sub;  // 处理订阅
// static rclc_executor_t executor_pub;  // 处理发布和定时器
// static rcl_node_t node;
// static rcl_timer_t timer;
// static int countt;
// static rcl_ret_t temp_ret;

// /* Publishers */
// static rcl_publisher_t right_wheel_feedback_publisher;
// static rcl_publisher_t left_wheel_feedback_publisher;
// static rcl_publisher_t right_wheel_target_publisher;
// static rcl_publisher_t left_wheel_target_publisher;

// /* Publisher messages */
// static std_msgs__msg__Float64 right_wheel_feedback_msg;
// static std_msgs__msg__Float64 left_wheel_feedback_msg;
// static std_msgs__msg__Float64 right_wheel_target_msg;
// static std_msgs__msg__Float64 left_wheel_target_msg;

// /* Subscribers */
// static rcl_subscription_t motor1_subscriber;
// static rcl_subscription_t motor2_subscriber;
// static rcl_subscription_t cmd_vel_subscriber;

// /* Subscriber messages */
// static std_msgs__msg__Float64 motor1_msg;
// static std_msgs__msg__Float64 motor2_msg;
// static geometry_msgs__msg__Twist cmd_vel_msg;

// /* Monitoring counters */
// static volatile uint32_t executor_count = 0;
// static volatile uint32_t callback_count = 0;
// static volatile uint32_t timer_count = 0;

// /* Helper functions */
// static void calculate_wheel_speeds(float linear_x, float angular_z, float *left_rpm, float *right_rpm) {
//     float left_wheel_speed = (linear_x - angular_z * WHEEL_SEPARATION / 2.0f) / WHEEL_RADIUS;
//     float right_wheel_speed = (linear_x + angular_z * WHEEL_SEPARATION / 2.0f) / WHEEL_RADIUS;
    
//     *left_rpm = left_wheel_speed * RADS_TO_RPM;
//     *right_rpm = right_wheel_speed * RADS_TO_RPM;
// }

// /* 执行器健康检查 */
// static void check_executor_health(void) {
//     uint32_t now = HAL_GetTick();
    
//     // 检查执行器是否长时间未运行
//     if(now - last_executor_run > EXECUTOR_RESET_INTERVAL) {
//         executor_healthy = false;
//         executor_errors++;
//     }
    
//     // 检查队列状态
//     if(current_queue >= QUEUE_WARNING_THRESHOLD) {
//         queue_warning = true;
//     } else {
//         queue_warning = false;
//     }
// }

// /* 重置执行器 */
// static void reset_executors_if_needed(void) {
//     if(!executor_healthy || queue_warning) {
//         rclc_executor_prepare(&executor_sub);
//         rclc_executor_prepare(&executor_pub);
//         executor_healthy = true;
//         queue_warning = false;
//         current_queue = 0;  // 重置队列计数
//     }
// }

// /* Callback Implementation */
// static void timer_callback(rcl_timer_t *timer, int64_t last_call_time) {
//     timer_count++;
//     if (timer != NULL) {
//         // 检查状态
//         check_executor_health();
        
//         // pingping 消息发布
//         msg.data = countt--;
//         temp_ret = rcl_publish(&publisher, (const void*)&msg, NULL);
//         if(temp_ret != RCL_RET_OK) {
//             publish_errors++;
//         }
        
//         // 准备所有数据
//         right_wheel_feedback_msg.data = hmotor1.current_rpm;
//         right_wheel_target_msg.data = hmotor1.target_rpm;
//         left_wheel_feedback_msg.data = hmotor2.current_rpm;
//         left_wheel_target_msg.data = hmotor2.target_rpm;
        
//         // 批量发布
//         temp_ret = rcl_publish(&right_wheel_feedback_publisher, &right_wheel_feedback_msg, NULL);
//         if(temp_ret == RCL_RET_OK) {
//             temp_ret = rcl_publish(&right_wheel_target_publisher, &right_wheel_target_msg, NULL);
//             if(temp_ret == RCL_RET_OK) {
//                 temp_ret = rcl_publish(&left_wheel_feedback_publisher, &left_wheel_feedback_msg, NULL);
//                 if(temp_ret == RCL_RET_OK) {
//                     temp_ret = rcl_publish(&left_wheel_target_publisher, &left_wheel_target_msg, NULL);
//                 }
//             }
//         }
        
//         if(temp_ret != RCL_RET_OK) {
//             publish_errors++;
//             if(publish_errors > 5) {  // 连续错误超过阈值
//                 reset_executors_if_needed();
//                 publish_errors = 0;
//             }
//         } else {
//             publish_errors = 0;  // 成功发布则重置错误计数
//         }
//     }
// }

// static void motor1_callback(const void * msgin) {
//     if(current_queue >= MAX_QUEUED_MESSAGES) {
//         dropped_messages++;
//         return;  // 队列满则丢弃
//     }
    
//     total_messages++;
//     current_queue++;
    
//     if(current_queue > max_queue) {
//         max_queue = current_queue;
//     }
    
//     callback_count++;
//     const std_msgs__msg__Float64 * msg = (const std_msgs__msg__Float64 *)msgin;
//     float target_rpm = (float)msg->data;
//     Motor_SetTargetSpeed(&hmotor1, target_rpm);
    
//     processed_messages++;
//     current_queue--;
// }

// static void motor2_callback(const void * msgin) {
//     if(current_queue >= MAX_QUEUED_MESSAGES) {
//         dropped_messages++;
//         return;
//     }
    
//     total_messages++;
//     current_queue++;
    
//     callback_count++;
//     const std_msgs__msg__Float64 * msg = (const std_msgs__msg__Float64 *)msgin;
//     float target_rpm = (float)msg->data;
//     Motor_SetTargetSpeed(&hmotor2, target_rpm);
    
//     processed_messages++;
//     current_queue--;
// }

// static void cmd_vel_callback(const void * msgin) {
//     if(current_queue >= MAX_QUEUED_MESSAGES) {
//         dropped_messages++;
//         return;
//     }
    
//     total_messages++;
//     current_queue++;
    
//     const geometry_msgs__msg__Twist * msg = (const geometry_msgs__msg__Twist *)msgin;
//     float linear_x = (float)msg->linear.x;
//     float angular_z = (float)msg->angular.z;
    
//     linear_x = fminf(fmaxf(linear_x, -MAX_LINEAR_SPEED), MAX_LINEAR_SPEED);
//     angular_z = fminf(fmaxf(angular_z, -MAX_ANGULAR_SPEED), MAX_ANGULAR_SPEED);
    
//     float left_rpm, right_rpm;
//     calculate_wheel_speeds(linear_x, angular_z, &left_rpm, &right_rpm);
    
//     Motor_SetTargetSpeed(&hmotor1, left_rpm);
//     Motor_SetTargetSpeed(&hmotor2, right_rpm);
    
//     processed_messages++;
//     current_queue--;
// }

// /* MicroROS initialization */
// void MicroROS_Init(void) {
//     rmw_uros_set_custom_transport(
//         true,
//         (void *)&huart1,
//         cubemx_transport_open,
//         cubemx_transport_close,
//         cubemx_transport_write,
//         cubemx_transport_read
//     );

//     rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
//     freeRTOS_allocator.allocate = microros_allocate;
//     freeRTOS_allocator.deallocate = microros_deallocate;
//     freeRTOS_allocator.reallocate = microros_reallocate;
//     freeRTOS_allocator.zero_allocate = microros_zero_allocate;

//     if (!rcutils_set_default_allocator(&freeRTOS_allocator)) {
//         debug_print("Error on default allocators\r\n");
//     }

//     allocator = rcl_get_default_allocator();
//     rclc_support_init(&support, 0, NULL, &allocator);
//     rclc_node_init_default(&node, "cubemx_node", "", &support);

//     // 初始化两个执行器，减小handle数量
//     // rclc_executor_init(&executor_sub, &support.context, 3, &allocator); // 3个订阅
//     // rclc_executor_init(&executor_pub, &support.context, 1, &allocator); // 1个定时器

//     rclc_executor_init(&executor_sub, &support.context, 4, &allocator);  // 3+2 预留
//     rclc_executor_init(&executor_pub, &support.context, 6, &allocator);  // 1+1 预留


//     // Initialize publishers
//     rclc_publisher_init_default(
//         &publisher,
//         &node,
//         ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
//         "ping_ping"
//     );

//     rclc_publisher_init_default(
//         &right_wheel_feedback_publisher,
//         &node,
//         ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
//         "wheel_right/feedback"
//     );
    
//     rclc_publisher_init_default(
//         &right_wheel_target_publisher,
//         &node,
//         ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
//         "wheel_right/target"
//     );
    
//     rclc_publisher_init_default(
//         &left_wheel_feedback_publisher,
//         &node,
//         ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
//         "wheel_left/feedback"
//     );
    
//     rclc_publisher_init_default(
//         &left_wheel_target_publisher,
//         &node,
//         ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
//         "wheel_left/target"
//     );

//     // Initialize subscribers
//     rclc_subscription_init_default(
//         &motor1_subscriber,
//         &node,
//         ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
//         "wheel_right/target_speed"
//     );

//     rclc_subscription_init_default(
//         &motor2_subscriber,
//         &node,
//         ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
//         "wheel_left/target_speed"
//     );

//     rclc_subscription_init_default(
//         &cmd_vel_subscriber,
//         &node,
//         ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
//         "cmd_vel"
//     );

//     // Initialize timer with longer period
//     rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(100), timer_callback);

//     // 添加到各自的执行器
//     rclc_executor_add_timer(&executor_pub, &timer);
//     rclc_executor_add_subscription(&executor_sub, &motor1_subscriber, &motor1_msg, &motor1_callback, ON_NEW_DATA);
//     rclc_executor_add_subscription(&executor_sub, &motor2_subscriber, &motor2_msg, &motor2_callback, ON_NEW_DATA);
//     rclc_executor_add_subscription(&executor_sub, &cmd_vel_subscriber, &cmd_vel_msg, &cmd_vel_callback, ON_NEW_DATA);

//     // Initialize motors and encoders
//     Motors_Init();
//     Encoders_Init();
    
//     // 初始化监控变量
//     executor_healthy = true;
//     queue_warning = false;
//     last_executor_run = HAL_GetTick();
// }

// /* MicroROS task implementation */
// void MicroROS_TaskStart(void *argument) {
//     uint32_t last_print_time = 0;
//     uint32_t last_sub_time = 0;
//     uint32_t last_pub_time = 0;
//     uint32_t last_reset_time = 0;
//     uint32_t last_health_check = 0;
//     char num_str[20];
//     rcl_ret_t ret;
    
//     for (;;) {
//         uint32_t now = HAL_GetTick();
        
//         // 定期健康检查
//         if(now - last_health_check >= 1000) {
//             check_executor_health();
//             if(!executor_healthy || queue_warning) {
//                 reset_executors_if_needed();
//             }
//             last_health_check = now;
//         }
        
//         // 处理订阅消息（高频率）
//         if(now - last_sub_time >= 1) {
//             executor_count++;
//             ret = rclc_executor_spin_some(&executor_sub, RCL_MS_TO_NS(1));
//             if(ret == RCL_RET_OK) {
//                 last_executor_run = now;  // 更新最后成功执行时间
//             }
//             last_sub_time = now;
//         }
        
//         // 处理发布消息（较低频率）
//         if(now - last_pub_time >= 2) {
//             ret = rclc_executor_spin_some(&executor_pub, RCL_MS_TO_NS(1));
//             if(ret == RCL_RET_OK) {
//                 last_executor_run = now;
//             }
//             last_pub_time = now;
//         }
        
//         // 定期重置计数器（防止溢出）
//         if(now - last_reset_time >= 10000) {
//             if(current_queue > 0) {  // 如果还有未处理的消息，记录错误
//                 executor_errors++;
//             }
//             total_messages = 0;
//             processed_messages = 0;
//             current_queue = 0;
//             max_queue = 0;
//             last_reset_time = now;
//         }
        
//         // 打印统计信息
//         if(now - last_print_time >= 1000) {
//             debug_print("\r\n--- Stats ---\r\n");
            
//             debug_print("Exec: ");
//             num_to_str(executor_count, num_str);
//             debug_print(num_str);
//             debug_print("\r\n");
            
//             debug_print("Callback: ");
//             num_to_str(callback_count, num_str);
//             debug_print(num_str);
//             debug_print("\r\n");
            
//             debug_print("Timer: ");
//             num_to_str(timer_count, num_str);
//             debug_print(num_str);
//             debug_print("\r\n");
            
//             debug_print("Total: ");
//             num_to_str(total_messages, num_str);
//             debug_print(num_str);
//             debug_print("\r\n");
            
//             debug_print("Processed: ");
//             num_to_str(processed_messages, num_str);
//             debug_print(num_str);
//             debug_print("\r\n");
            
//             debug_print("Queue: ");
//             num_to_str(current_queue, num_str);
//             debug_print(num_str);
//             debug_print("\r\n");
            
//             debug_print("MaxQ: ");
//             num_to_str(max_queue, num_str);
//             debug_print(num_str);
//             debug_print("\r\n");
            
//             debug_print("Dropped: ");
//             num_to_str(dropped_messages, num_str);
//             debug_print(num_str);
//             debug_print("\r\n");
            
//             debug_print("Errors: ");
//             num_to_str(executor_errors, num_str);
//             debug_print(num_str);
//             debug_print("\r\n");
            
//             // 重置部分计数器
//             executor_count = 0;
//             callback_count = 0;
//             timer_count = 0;
//             last_print_time = now;
//         }
        
//         osDelay(1);
//     }
// }

#include "microros_task.h"
#include "gpio.h"
#include "usart.h"
#include "cmsis_os.h"

// FreeRTOS Headers
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/float64.h>
#include <rmw_microros/rmw_microros.h>
#include <rmw/rmw.h>

/* Debug Flags */
volatile struct {
    uint32_t last_pub_time;
    uint32_t last_sub_time;
    uint32_t pub_success_count;
    uint32_t pub_fail_count;
    uint32_t sub_success_count;
    uint32_t sub_fail_count;
    uint32_t queue_full_count;
    uint32_t mutex_timeout_count;
    uint32_t spin_fail_count;
    uint32_t transport_error_count;
    uint8_t pub_mutex_taken;
    uint8_t sub_mutex_taken;
    uint8_t transport_ok;
    uint8_t executor_ok;
    uint32_t queue_current_size;
    uint32_t last_error_code;
    uint32_t last_callback_time;
    uint32_t callback_duration;
    uint32_t spin_duration;
    uint32_t publish_duration;
} debug_flags = {0};

/* Error Codes */
#define ERROR_NONE              0
#define ERROR_MUTEX_TIMEOUT     1
#define ERROR_QUEUE_FULL        2
#define ERROR_PUBLISH_FAIL      3
#define ERROR_SPIN_FAIL         4
#define ERROR_TRANSPORT_FAIL    5
#define ERROR_EXECUTOR_FAIL     6

/* Constants */
#define QUEUE_SIZE 32
#define SPIN_INTERVAL 2
#define PUBLISH_INTERVAL 50

/* Message Queue */
typedef struct {
    float data;
    uint32_t timestamp;
} QueueItem;

/* Handles */
static QueueHandle_t msgQueue;
static osMutexId_t pub_mutex = NULL;
static osMutexId_t sub_mutex = NULL;
static StaticQueue_t queueBuffer;
static uint8_t queueStorageArea[QUEUE_SIZE * sizeof(QueueItem)];

/* ROS Variables */
static rcl_publisher_t counter_publisher;
static rcl_subscription_t command_subscriber;
static std_msgs__msg__Int32 pub_msg;
static std_msgs__msg__Float64 sub_msg;
static rclc_executor_t executor;
static rclc_support_t support;
static rcl_allocator_t allocator;
static rcl_node_t node;

/* Command Callback */
static void command_callback(const void * msgin) {
    uint32_t start_time = HAL_GetTick();
    debug_flags.last_callback_time = start_time;
    
    if (osMutexAcquire(sub_mutex, 10) != osOK) {
        debug_flags.mutex_timeout_count++;
        debug_flags.last_error_code = ERROR_MUTEX_TIMEOUT;
        return;
    }
    
    debug_flags.sub_mutex_taken = 1;
    
    const std_msgs__msg__Float64 * msg = (const std_msgs__msg__Float64 *)msgin;
    QueueItem item = {
        .data = msg->data,
        .timestamp = start_time
    };
    
    UBaseType_t spaces = uxQueueSpacesAvailable(msgQueue);
    debug_flags.queue_current_size = QUEUE_SIZE - spaces;
    
    if (xQueueSend(msgQueue, &item, 0) != pdPASS) {
        debug_flags.queue_full_count++;
        debug_flags.last_error_code = ERROR_QUEUE_FULL;
        
        // 清空一半队列
        QueueItem dummy;
        for(int i = 0; i < QUEUE_SIZE/2; i++) {
            xQueueReceive(msgQueue, &dummy, 0);
        }
    } else {
        debug_flags.sub_success_count++;
        debug_flags.last_sub_time = start_time;
    }
    
    debug_flags.sub_mutex_taken = 0;
    osMutexRelease(sub_mutex);
    
    debug_flags.callback_duration = HAL_GetTick() - start_time;
}

/* MicroROS initialization */
void MicroROS_Init(void) {
    // 初始化调试标志
    memset((void*)&debug_flags, 0, sizeof(debug_flags));
    debug_flags.transport_ok = 1;
    debug_flags.executor_ok = 1;
    
    const osMutexAttr_t mutex_attributes = {
        .name = "ros_mutex",
        .attr_bits = osMutexPrioInherit,
        .cb_mem = NULL,
        .cb_size = 0U
    };
    
    pub_mutex = osMutexNew(&mutex_attributes);
    sub_mutex = osMutexNew(&mutex_attributes);
    
    // 创建消息队列
    msgQueue = xQueueCreateStatic(QUEUE_SIZE, 
                                 sizeof(QueueItem),
                                 queueStorageArea,
                                 &queueBuffer);
    
    if (!pub_mutex || !sub_mutex || !msgQueue) {
        debug_flags.last_error_code = ERROR_EXECUTOR_FAIL;
        return;
    }
    
    allocator = rcl_get_default_allocator();
    
    // 设置传输层
    rmw_uros_set_custom_transport(
        true,
        (void *)&huart1,
        cubemx_transport_open,
        cubemx_transport_close,
        cubemx_transport_write,
        cubemx_transport_read
    );
    
    rclc_support_init(&support, 0, NULL, &allocator);
    rclc_node_init_default(&node, "minimal_node", "", &support);
    rclc_executor_init(&executor, &support.context, 1, &allocator);
    rclc_executor_set_timeout(&executor, RCL_MS_TO_NS(5));
    
    // 初始化发布者
    rclc_publisher_init_default(
        &counter_publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "counter"
    );
    
    // 初始化订阅者
    rclc_subscription_init_default(
        &command_subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
        "command"
    );
    
    rclc_executor_add_subscription(
        &executor, 
        &command_subscriber, 
        &sub_msg, 
        &command_callback,
        ON_NEW_DATA
    );
}

/* MicroROS task implementation */
void MicroROS_TaskStart(void *argument) {
    uint32_t last_pub_time = 0;
    uint32_t last_spin_time = 0;
    int32_t count = 0;
    QueueItem received_item;
    rcl_ret_t ret;
    
    for (;;) {
        uint32_t now = HAL_GetTick();
        
        // 检查传输层状态
        if (rmw_uros_ping_agent(100, 2) != RMW_RET_OK) {
            debug_flags.transport_ok = 0;
            debug_flags.transport_error_count++;
            debug_flags.last_error_code = ERROR_TRANSPORT_FAIL;
        } else {
            debug_flags.transport_ok = 1;
        }
        
        // 处理订阅消息
        if (now - last_spin_time >= SPIN_INTERVAL && debug_flags.transport_ok) {
            uint32_t spin_start = HAL_GetTick();
            
            if (osMutexAcquire(sub_mutex, 10) == osOK) {
                debug_flags.sub_mutex_taken = 1;
                ret = rclc_executor_spin_some(&executor, RCL_MS_TO_NS(1));
                if (ret != RCL_RET_OK) {
                    debug_flags.spin_fail_count++;
                    debug_flags.last_error_code = ERROR_SPIN_FAIL;
                }
                debug_flags.sub_mutex_taken = 0;
                osMutexRelease(sub_mutex);
            } else {
                debug_flags.mutex_timeout_count++;
            }
            
            debug_flags.spin_duration = HAL_GetTick() - spin_start;
            last_spin_time = now;
        }
        
        // 发布消息
        if (now - last_pub_time >= PUBLISH_INTERVAL && debug_flags.transport_ok) {
            uint32_t pub_start = HAL_GetTick();
            
            if (osMutexAcquire(pub_mutex, 10) == osOK) {
                debug_flags.pub_mutex_taken = 1;
                pub_msg.data = count++;
                ret = rcl_publish(&counter_publisher, &pub_msg, NULL);
                if (ret == RCL_RET_OK) {
                    debug_flags.pub_success_count++;
                    debug_flags.last_pub_time = now;
                } else {
                    debug_flags.pub_fail_count++;
                    debug_flags.last_error_code = ERROR_PUBLISH_FAIL;
                }
                debug_flags.pub_mutex_taken = 0;
                osMutexRelease(pub_mutex);
            } else {
                debug_flags.mutex_timeout_count++;
            }
            
            debug_flags.publish_duration = HAL_GetTick() - pub_start;
            last_pub_time = now;
        }
        
        // 处理队列中的消息
        while (xQueueReceive(msgQueue, &received_item, 0) == pdPASS) {
            // 处理消息
        }
        
        osDelay(1);
    }
}