#ifndef __STEP_MOTOR_H__
#define __STEP_MOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "include.h"
void control_motor_init(void);
void control_motor_speed(uint8_t motor_id, float speed);

#endif