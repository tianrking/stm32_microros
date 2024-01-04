#include "include.h"
#include <std_msgs/msg/int32.h>
#include <rmw/rmw.h>  // 包含rmw_qos_profile_default

extern  rclc_support_t support;
extern  rcl_allocator_t allocator;
extern  rcl_node_t node;
extern  rclc_executor_t executor;

// 全局变量
rcl_publisher_t current_state_publisher;
std_msgs__msg__Int32 current_state_msg;

// 发布状态的函数
void publish_current_state(int32_t current_state) 
{    // 设置消息数据
    // int32_t current_state = 99;
    current_state_msg.data = current_state;

    // 发布消息
    rcl_ret_t rc = rcl_publish(&current_state_publisher, &current_state_msg, NULL);
    if (rc != RCL_RET_OK) {
        printf("Failed to publish current state.\n");
    }
}

// void setup_publisher(void) {
//     // 初始化发布
//     rcl_publisher_options_t pub_opt = rcl_publisher_get_default_options();
//     rcl_ret_t rc = rcl_publisher_init(
//         &current_state_publisher,
//         &node,
//         ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
//         "mcu_current_state",
//         &pub_opt);

//     if (rc != RCL_RET_OK) {
//         // 错误处理：输出错误并返回
//         printf("Failed to initialize psublisher: %s\n", rcl_get_error_string().str);
//         rcl_reset_error();
//     }
// }

// void setup_publisher(void) {
//     // 初始化发布
//     rcl_publisher_options_t pub_opt = rcl_publisher_get_default_options();
    
//     // 修改QoS设置
//     pub_opt.qos.reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE;
//     pub_opt.qos.history = RMW_QOS_POLICY_HISTORY_KEEP_LAST;
//     pub_opt.qos.depth = 1; // 保持最后一个消息

//     rcl_ret_t rc = rcl_publisher_init(
//         &current_state_publisher,
//         &node,
//         ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
//         "mcu_current_state",
//         &pub_opt);

//     if (rc != RCL_RET_OK) {
//         // 错误处理：输出错误并返回
//         printf("Failed to initialize publisher: %s\n", rcl_get_error_string().str);
//         rcl_reset_error();
//     }
// }

