#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "gpio.h"
#include "usart.h"
#include "tim.h"
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <uxr/client/transport.h>
#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros/rmw_microros.h>
#include <std_msgs/msg/int32.h>

#include "move_motor.h"
#include "step_motor.h"

#include "mc_include.h"
#include "spi_240_240.h"
#include "emm_motor.h"
#include "emm_motor_rsp.h"

#endif