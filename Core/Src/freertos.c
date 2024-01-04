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
void Configure_TIM4_PWM_Frequency(uint32_t input);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <uxr/client/transport.h>
#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros/rmw_microros.h>

#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/int32_multi_array.h>
#include "include.h"
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
bool cubemx_transport_open(struct uxrCustomTransport *transport);
bool cubemx_transport_close(struct uxrCustomTransport *transport);
size_t cubemx_transport_write(struct uxrCustomTransport *transport, const uint8_t *buf, size_t len, uint8_t *err);
size_t cubemx_transport_read(struct uxrCustomTransport *transport, uint8_t *buf, size_t len, int timeout, uint8_t *err);

void *microros_allocate(size_t size, void *state);
void microros_deallocate(void *pointer, void *state);
void *microros_reallocate(void *pointer, size_t size, void *state);
void *microros_zero_allocate(size_t number_of_elements, size_t size_of_element, void *state);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
rcl_publisher_t publisher;
std_msgs__msg__Int32 msg;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rclc_executor_t executor;

// 定义全局订阅者和消息
rcl_subscription_t status_set_subscription;
std_msgs__msg__Int32 status_set_msg;
rcl_subscription_t wheel_speeds_subscription;
std_msgs__msg__Int32MultiArray wheel_speeds_msg;

// 定义回调函数
void status_set_callback(const void *msgin);
void wheel_speeds_callback(const void *msgin);

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
uint32_t defaultTaskBuffer[7000];
osStaticThreadDef_t defaultTaskControlBlock;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .cb_mem = &defaultTaskControlBlock,
    .cb_size = sizeof(defaultTaskControlBlock),
    .stack_mem = &defaultTaskBuffer[0],
    .stack_size = sizeof(defaultTaskBuffer),
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for PWM_Task */
osThreadId_t PWM_TaskHandle;
uint32_t pwmcontroltaskBuffer[3000];
osStaticThreadDef_t pwmcontroltaskControlBlock;
const osThreadAttr_t PWM_Task_attributes = {
    .name = "PWM_Task",
    .cb_mem = &pwmcontroltaskControlBlock,
    .cb_size = sizeof(pwmcontroltaskControlBlock),
    .stack_mem = &pwmcontroltaskBuffer[0],
    .stack_size = sizeof(pwmcontroltaskBuffer),
    .priority = (osPriority_t)osPriorityLow,
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
void MX_FREERTOS_Init(void)
{
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

  /* creation of PWM_Task */
  PWM_TaskHandle = osThreadNew(StartTask02, NULL, &PWM_Task_attributes);

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

rcl_publisher_t new_publisher; // 新的publisher
std_msgs__msg__Int32 new_msg;  // 新的消息类型

extern rcl_publisher_t current_state_publisher;
extern std_msgs__msg__Int32 current_state_msg;
extern int desired_state;
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  rmw_uros_set_custom_transport(
      true,
      (void *)&huart2,
      cubemx_transport_open,
      cubemx_transport_close,
      cubemx_transport_write,
      cubemx_transport_read);

  rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
  freeRTOS_allocator.allocate = microros_allocate;
  freeRTOS_allocator.deallocate = microros_deallocate;
  freeRTOS_allocator.reallocate = microros_reallocate;
  freeRTOS_allocator.zero_allocate = microros_zero_allocate;

  if (!rcutils_set_default_allocator(&freeRTOS_allocator))
  {
    printf("Error on default allocators (line %d)\n", __LINE__);
  }

  // micro-ROS app

  // rcl_publisher_t publisher;
  // std_msgs__msg__Int32 msg;
  // rclc_support_t support;
  // rcl_allocator_t allocator;
  // rcl_node_t node;

  allocator = rcl_get_default_allocator();

  // create init_options
  rclc_support_init(&support, 0, NULL, &allocator);

  // create node
  rclc_node_init_default(&node, "cubemx_node", "", &support);

  // create publisher
  rclc_publisher_init_default(
      &publisher,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
      "cubemx_publisher");

  msg.data = 0;
  // control_motor_init();  //PWM init

  // setup_subscriber();
  //  setup_publisher();

  rcl_publisher_t new_publisher; // 新的publisher
  std_msgs__msg__Int32 new_msg;  // 新的消息类型

  // 初始化新的publisher
  rclc_publisher_init_default(
      &new_publisher,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
      "new_topic"); // 新话题

  rclc_publisher_init_default(
      &current_state_publisher,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
      "mcu_current_state"); // 新话题

  // 初始化状态设置订阅
  rcl_subscription_options_t status_set_subscription_ops = rcl_subscription_get_default_options();
  rcl_ret_t sub_ret = rcl_subscription_init(
      &status_set_subscription,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
      "set_mcu_state",
      &status_set_subscription_ops);

  if (sub_ret != RCL_RET_OK)
  {
    // 错误处理
    printf("Error initializing 'set_mcu_state' subscription (line %d)\n", __LINE__);
  }

  // 初始化电机速度订阅
  rcl_subscription_options_t wheel_speeds_subscription_ops = rcl_subscription_get_default_options();
  sub_ret = rcl_subscription_init(
      &wheel_speeds_subscription,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32MultiArray),
      "wheel_speeds",
      &wheel_speeds_subscription_ops);

  if (sub_ret != RCL_RET_OK)
  {
    // 错误处理
    printf("Error initializing 'wheel_speeds' subscription (line %d)\n", __LINE__);
  }

  // 添加状态设置订阅者到执行器
  rclc_executor_add_subscription(
      &executor, 
      &status_set_subscription, 
      &status_set_msg, 
      status_set_callback, 
      ON_NEW_DATA);

  // 添加电机速度订阅者到执行器
  rclc_executor_add_subscription(
      &executor, 
      &wheel_speeds_subscription, 
      &wheel_speeds_msg, 
      NULL, // 确保这里不是NULL，应该是正确的回调函数
      ON_NEW_DATA);


  new_msg.data = 0;
  current_state_msg.data = 0;
  /* Infinite loop */
  for (;;)
  {
    rcl_ret_t ret = rcl_publish(&publisher, &msg, NULL);

    if (ret != RCL_RET_OK)
    {
      printf("Error publishing (line %d)\n", __LINE__);
    }
    // int32_t pp = '7';

    msg.data++;

    // 发布新消息
    new_msg.data += 9; // 更新新消息内容
    rcl_ret_t new_ret = rcl_publish(&new_publisher, &new_msg, NULL);
    if (new_ret != RCL_RET_OK)
    {
      printf("Error publishing new message (line %d)\n", __LINE__);
    }

    // current_state_msg.data = desired_state;
    ret = rcl_publish(&current_state_publisher, &current_state_msg, NULL);
    // if(msg.data % 100 == 0){
    //   // publish_current_state(msg.data);
    //   publish_current_state();
    // }
    // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    // osDelay(200);
    // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    // osDelay(200);

    // control_motor_speed(0,200);
    // control_motor_speed(1,-800);
    // HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
    osDelay(1000);
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(1000));
  }
  // rcl_service_fini(&service, &node);
  // rcl_node_fini(&node);
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
 * @brief Function implementing the PWM_Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);

  TIM4->CCR1 = 100; // PD12
  TIM4->CCR2 = 400; // PD13
  TIM4->CCR3 = 600; // PD14
  TIM4->CCR4 = 800; // PD15
  // static int pre = 84;
  // static int delayTime = 2000;
  /* Infinite loop */
  for (;;)
  {

    // 逐渐增加频率
    for (int value = 0; value <= 50; value++)
    {
      Configure_TIM4_PWM_Frequency(value);
      osDelay(100);
    }

    // 逐渐减少频率
    for (int value = 50; value > 0; value--)
    {
      Configure_TIM4_PWM_Frequency(value);
      osDelay(100);
    }
  }
  /* USER CODE END StartTask02 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void Configure_TIM4_PWM_Frequency(uint32_t input)
{
  uint32_t prescaler = 1;              // Prescaler is 1
  uint32_t max_frequency = 13000;      // 13kHz
  uint32_t min_frequency = 0;          // 0Hz
  uint32_t clock_frequency = 84000000; // Assuming 84MHz Timer Clock frequency

  // Map input 0-100 to 0-13kHz
  uint32_t target_frequency = ((max_frequency - min_frequency) * input / 100) + min_frequency;

  // Calculate period based on the target frequency
  uint32_t period = (clock_frequency / ((prescaler + 1) * target_frequency)) - 1;

  /* Stop TIM4 to set the new configuration */
  HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_ALL);

  /* Set the new prescaler and period values */
  htim4.Init.Prescaler = prescaler - 1; // Prescaler value in register is 1 less than actual value
  htim4.Init.Period = period;

  /* Re-initialize TIM4 with new settings */
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }

  /* Start TIM4 with the new frequency settings */
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
}

void status_set_callback(const void *msgin)
{
  const std_msgs__msg__Int32 *msg = msgin;
  current_state_msg.data = msg->data;
  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
  // printf("%d",desired_state);
  //  处理设置状态
  // int32_t current_state = handle_set_state(desired_state);
  // publish_current_state(desired_state);

  // current_state_msg.data = desired_state;
  // ret = rcl_publish(&current_state_publisher, msg, NULL);
  //  这里可能需要发布当前状态，根据具体情况来调用 publish_current_state(current_state);
}

/* USER CODE END Application */
