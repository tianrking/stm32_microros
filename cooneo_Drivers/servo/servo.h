#ifndef __SERVO_H__
#define __SERVO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "include.h"
void Set_Servo_Angle(uint8_t servo_id, float angle);
void Servos_Init(void);
#endif