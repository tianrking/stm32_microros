#ifndef __STEP_MOTOR_H__
#define __STEP_MOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

// #include "FreeRTOS.h"
// #include "task.h"
// #include "main.h"
// #include "cmsis_os.h"

// #include "usart.h"
// #include "gpio.h"
// #include "tim.h"
// #include <rcl/rcl.h>
// #include <rcl/error_handling.h>
// #include <rclc/rclc.h>
// #include <rclc/executor.h>
// #include <uxr/client/transport.h>
// #include <rmw_microxrcedds_c/config.h>
// #include <rmw_microros/rmw_microros.h>

// #include <std_msgs/msg/int32.h>
#include "include.h"

// 假设步进电机步进角为1.8度，每圈需要200个脉冲
#define STEP_ANGLE 1.8
#define STEPS_PER_REV 200
void Configure_TIM2_PWM_Frequency(uint32_t input);
void rotate_motor_by_angle(float angle);
#endif