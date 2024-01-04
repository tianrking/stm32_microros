// #include "include.h"

// #include <stdio.h>
// #include <string.h>

// extern  rclc_support_t support;
// extern  rcl_allocator_t allocator;
// extern  rcl_node_t node;
// extern rclc_executor_t executor;

// rcl_service_t service;
// std_msgs__msg__Int32 last_request;
// std_msgs__msg__Int32 last_response;

// // 定义操作枚举
// typedef enum {
//     OPERATION_A,
//     OPERATION_B,
//     OPERATION_C,
//     // ... 添加更多操作 ...
//     OPERATION_COUNT // 这个总是最后一个，用来表示操作的数量
// } OperationType;

// // 定义操作结构体
// typedef struct {
//     OperationType type;  // 操作类型
//     void (*execute)(void);  // 执行操作的函数指针
//     const char *description;  // 操作描述
// } Operation;

// // 实现每个操作的函数
// void executeOperationA(void) { printf("Executing Operation A\n"); }
// void executeOperationB(void) { printf("Executing Operation B\n"); }
// void executeOperationC(void) { printf("Executing Operation C\n"); }
// // ... 添加更多操作的实现 ...

// // 创建一个操作数组
// Operation operations[OPERATION_COUNT] = {
//     {OPERATION_A, executeOperationA, "Operation A"},
//     {OPERATION_B, executeOperationB, "Operation B"},
//     {OPERATION_C, executeOperationC, "Operation C"},
//     // ... 添加更多操作 ...
// };

// // 服务回调函数
// void service_callback(const void * req, void * res){
//     const std_msgs__msg__Int32 * request = (const std_msgs__msg__Int32 *) req;
//     std_msgs__msg__Int32 * response = (std_msgs__msg__Int32 *) res;

//     int operation_index = request->data;  // 请求中的数据表示操作索引

//     if (operation_index >= 0 && operation_index < OPERATION_COUNT) {
//         // 执行请求的操作
//         operations[operation_index].execute();
//         response->data = operation_index;  // 返回执行的操作索引
//         printf("Response: Executed %s\n", operations[operation_index].description);
//     } else {
//         // 处理错误的请求
//         printf("Error: Received invalid operation index %d\n", operation_index);
//         response->data = -1;  // 返回错误标志
//     }
// }


// // 初始化和启动服务的函数
// void setup_service() {

//      rcl_service_options_t service_ops = rcl_service_get_default_options();
//     rcl_ret_t ret = rcl_service_init(&service, &node, ROSIDL_GET_SRV_TYPE_SUPPORT(std_msgs, msg, Int32), "my_service", &service_ops);

//     if (ret != RCL_RET_OK) {
//         // 错误处理：输出错误并返回
//         printf("Failed to initialize service: %s\n", rcl_get_error_string().str);
//         rcl_reset_error();
//         return;
//     }

//     // 添加服务到执行器
//     rclc_executor_add_service(&executor, &service, &last_request, &last_response, service_callback);

// }
