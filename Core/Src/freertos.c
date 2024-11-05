/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gpio.h"
#include "usart.h"
#include "tim.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#include <rcl/error_handling.h>
#include <rcl/rcl.h>
#include <rclc/executor.h>
#include <rclc/rclc.h>
#include <rmw_microros/rmw_microros.h>
#include <rmw_microxrcedds_c/config.h>
#include <uxr/client/transport.h>

#include <std_msgs/msg/int32.h>

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
bool cubemx_transport_open(struct uxrCustomTransport *transport);
bool cubemx_transport_close(struct uxrCustomTransport *transport);
size_t cubemx_transport_write(struct uxrCustomTransport *transport,
                              const uint8_t *buf, size_t len, uint8_t *err);
size_t cubemx_transport_read(struct uxrCustomTransport *transport, uint8_t *buf,
                             size_t len, int timeout, uint8_t *err);

void *microros_allocate(size_t size, void *state);
void microros_deallocate(void *pointer, void *state);
void *microros_reallocate(void *pointer, size_t size, void *state);
void *microros_zero_allocate(size_t number_of_elements, size_t size_of_element,
                             void *state);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
uint32_t defaultTaskBuffer[ 6000 ];
osStaticThreadDef_t defaultTaskControlBlock;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .cb_mem = &defaultTaskControlBlock,
  .cb_size = sizeof(defaultTaskControlBlock),
  .stack_mem = &defaultTaskBuffer[0],
  .stack_size = sizeof(defaultTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myTask02 */
osThreadId_t myTask02Handle;
uint32_t myTask02Buffer[ 2000 ];
osStaticThreadDef_t myTask02ControlBlock;
const osThreadAttr_t myTask02_attributes = {
  .name = "myTask02",
  .cb_mem = &myTask02ControlBlock,
  .cb_size = sizeof(myTask02ControlBlock),
  .stack_mem = &myTask02Buffer[0],
  .stack_size = sizeof(myTask02Buffer),
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask02(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of myTask02 */
  myTask02Handle = osThreadNew(StartTask02, NULL, &myTask02_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
rcl_publisher_t publisher;
std_msgs__msg__Int32 msg;
rclc_support_t support;
rcl_allocator_t allocator;
rclc_executor_t executor;
rcl_node_t node;
rcl_timer_t timer;
int countt;
rcl_ret_t temp_ret;
void timer_callback(rcl_timer_t *timer, int64_t last_call_time) {
  if (timer != NULL) {
    // 发布GPS数据
    msg.data = countt++; // float target_longitude
    temp_ret = rcl_publish(&publisher, &msg, NULL);
  }
}

#include "motor.h"
// 添加两个订阅者
rcl_subscription_t motor1_subscriber;
rcl_subscription_t motor2_subscriber;
// 添加电机控制消息
std_msgs__msg__Int32 motor1_msg;
std_msgs__msg__Int32 motor2_msg;

int32_t target_1_speed=0;
int32_t target_2_speed=0;
// 电机1回调函数
void motor1_callback(const void * msgin)
{
    const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
    target_1_speed = msg->data;
    
    // 限制输入范围在-1000到1000
    if (target_1_speed > 1000) target_1_speed = 1000;
    if (target_1_speed < -1000) target_1_speed = -1000;
    
    // 使用改进的电机驱动设置速度
    Motor_SetSpeed(&hmotor1, target_1_speed);
}

void motor2_callback(const void * msgin)
{
    const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
    target_2_speed = msg->data;
    
    // 限制输入范围在-1000到1000
    if (target_2_speed > 1000) target_2_speed = 1000;
    if (target_2_speed < -1000) target_2_speed = -1000;
    
    // 使用改进的电机驱动设置速度
    Motor_SetSpeed(&hmotor2, target_2_speed);
}


/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  rmw_uros_set_custom_transport(true, (void *)&huart1, cubemx_transport_open,
                                cubemx_transport_close, cubemx_transport_write,
                                cubemx_transport_read);

  rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
  freeRTOS_allocator.allocate = microros_allocate;
  freeRTOS_allocator.deallocate = microros_deallocate;
  freeRTOS_allocator.reallocate = microros_reallocate;
  freeRTOS_allocator.zero_allocate = microros_zero_allocate;

  if (!rcutils_set_default_allocator(&freeRTOS_allocator)) {
    printf("Error on default allocators (line %d)\n", __LINE__);
  }

  // micro-ROS app

  allocator = rcl_get_default_allocator();

  // create init_options
  rclc_support_init(&support, 0, NULL, &allocator);

  // create node
  rclc_node_init_default(&node, "cubemx_node", "", &support);
  rclc_executor_init(&executor, &support.context, 7, &allocator);

  // create publisher
  rclc_publisher_init_default(&publisher, &node,
                              ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
                              "ping_publisher");

  // 创建电机1订阅者
  rclc_subscription_init_default(
    &motor1_subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "motor1_control");

  // 创建电机2订阅者
  rclc_subscription_init_default(
    &motor2_subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "motor2_control");


  rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(100), timer_callback);

  rclc_executor_add_timer(&executor, &timer);

  rclc_executor_add_subscription(&executor, &motor1_subscriber, &motor1_msg, 
                                &motor1_callback, ON_NEW_DATA);
  rclc_executor_add_subscription(&executor, &motor2_subscriber, &motor2_msg,
                                &motor2_callback, ON_NEW_DATA);

  Motors_Init();
  Encoders_Init();

  /* Infinite loop */
  for (;;) {
    // rcl_ret_t ret = rcl_publish(&publisher, &msg, NULL);
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));

    msg.data++;
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    osDelay(200);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    osDelay(200);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask02 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

