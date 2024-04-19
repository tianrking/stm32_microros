// #include "microros_task.h"
// #include "FreeRTOS.h"
// #include "task.h"
// #include "cmsis_os.h"
// #include "main.h"
// #include "usart.h"
// #include "rcl/rcl.h"
// #include "rclc/rclc.h"
// #include "rclc/executor.h"
// #include "std_msgs/msg/int32.h"


// #include <rcl/error_handling.h>
// #include <uxr/client/transport.h>
// #include <rmw_microxrcedds_c/config.h>
// #include <rmw_microros/rmw_microros.h>

// #define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if (temp_rc != RCL_RET_OK) { error_loop(); } }
// #define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if (temp_rc != RCL_RET_OK) { printf("Failed status on line %d: %d. Continuing.\n", __LINE__, temp_rc); } }

// bool cubemx_transport_open(struct uxrCustomTransport * transport);
// bool cubemx_transport_close(struct uxrCustomTransport * transport);
// size_t cubemx_transport_write(struct uxrCustomTransport* transport, const uint8_t * buf, size_t len, uint8_t * err);
// size_t cubemx_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err);

// void * microros_allocate(size_t size, void * state);
// void microros_deallocate(void * pointer, void * state);
// void * microros_reallocate(void * pointer, size_t size, void * state);
// void * microros_zero_allocate(size_t number_of_elements, size_t size_of_element, void * state);

// rcl_allocator_t allocator;
// rclc_support_t support;
// rcl_node_t node;
// rclc_executor_t executor;

// rcl_publisher_t publisher;
// std_msgs__msg__Int32 msg;


// void error_loop(void)
// {
//     while(1) {
//         // Handle error, maybe blink an LED in a specific pattern or reset the microcontroller
//         osDelay(1000);
//     }
// }

// void micro_ros_task(void *argument)
// {

//     rmw_uros_set_custom_transport(
//       true,
//       (void *) &huart1,
//       cubemx_transport_open,
//       cubemx_transport_close,
//       cubemx_transport_write,
//       cubemx_transport_read);

//     rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
//     freeRTOS_allocator.allocate = microros_allocate;
//     freeRTOS_allocator.deallocate = microros_deallocate;
//     freeRTOS_allocator.reallocate = microros_reallocate;
//     freeRTOS_allocator.zero_allocate =  microros_zero_allocate;

//     if (!rcutils_set_default_allocator(&freeRTOS_allocator)) {
//         printf("Error on default allocators (line %d)\n", __LINE__);
//     }


//     allocator = rcl_get_default_allocator();
//     RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
//     RCCHECK(rclc_node_init_default(&node, "test", "", &support));

//     RCCHECK(rclc_publisher_init_default(
//         &publisher,
//         &node,
//         ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
//         "test_topic"));

//     // Executor initialization
//     RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));

//     int32_t count = 0; // Variable to store the incrementing integer

//     while(1) {
//         msg.data = count++;
//         RCCHECK(rcl_publish(&publisher, &msg, NULL));
//         osDelay(1000); // Delay for 1000ms or 1 second
//     }

//     // Cleanup if ever exit loop
//     rcl_publisher_fini(&publisher, &node);
//     rcl_node_fini(&node);
//     rclc_support_fini(&support);
// }
