// #include "include.h"

// #include <std_msgs/msg/int32_multi_array.h>

// extern  rclc_support_t support;
// extern  rcl_allocator_t allocator;
// extern  rcl_node_t node;
// extern rclc_executor_t executor;

// rcl_subscription_t subscriber;
// std_msgs__msg__Int32MultiArray wheel_speeds_msg;

// void wheel_speeds_callback(const void *msgin) {
//     const std_msgs__msg__Int32MultiArray *msg = (const std_msgs__msg__Int32MultiArray *)msgin;
//     if(msg->data.size >= 4) {  // 确保有足够的数据
//         for (int i = 0; i < 4; i++) {
//             control_motor_speed(i, msg->data.data[i]);  // 控制相应的电机
//         }
//     } else {
//         // 处理错误：接收到的数据不足4个速度值
//     }
// }

// int desired_state ;
// extern rcl_publisher_t current_state_publisher;
// // 订阅状态设置指令的回调函数
// void status_set_callback(const void * msgin) {
//     const std_msgs__msg__Int32 * msg = msgin;
//     desired_state = msg->data;
//     //printf("%d",desired_state);
//     // 处理设置状态
//     //int32_t current_state = handle_set_state(desired_state);
//     //publish_current_state(desired_state);

//     //current_state_msg.data = desired_state; 
//     //ret = rcl_publish(&current_state_publisher, msg, NULL);
//     // 这里可能需要发布当前状态，根据具体情况来调用 publish_current_state(current_state);
// }

// void setup_subscriber() {

//     // 初始化订阅
//     rcl_subscription_options_t subscription_ops = rcl_subscription_get_default_options();
//     rcl_ret_t ret = rcl_subscription_init(
//         &subscriber,
//         &node,
//         ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32MultiArray),
//         "wheel_speeds",
//         &subscription_ops);

//     if (ret != RCL_RET_OK) {
//         // 错误处理
//         printf("Failed to initialize subscriber: %s\n", rcl_get_error_string().str);
//     }

//     rcl_subscription_t status_set_subscription;
//     std_msgs__msg__Int32 status_set_msg;
//     subscription_ops = rcl_subscription_get_default_options();

//     rcl_ret_t rc = rcl_subscription_init(
//         &status_set_subscription,
//         &node,
//         ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
//         "set_mcu_state",
//         &subscription_ops);

//     if (rc != RCL_RET_OK) {
//         // 错误处理：输出错误并返回
//         printf("Failed to initialize subscription: %s\n", rcl_get_error_string().str);
//         rcl_reset_error();
//     }

//     //确保之前已经初始化了support，并且support包含了正确的上下文
//     rcl_context_t * context = &support.context;
//     // 初始化执行器
//     rclc_executor_init(&executor, context, 2, &allocator);

//     // 添加订阅到执行器
//     rclc_executor_add_subscription(
//         &executor,
//         &subscriber,
//         &wheel_speeds_msg,
//         &wheel_speeds_callback,
//         ON_NEW_DATA);

//     rclc_executor_add_subscription(
//         &executor, 
//         &status_set_subscription, 
//         &status_set_msg, 
//         &status_set_callback, 
//         ON_NEW_DATA);
// }
