// servo.h
#ifndef SERVO_H
#define SERVO_H

#include "main.h"

typedef struct {
    TIM_HandleTypeDef *htim;  // 定时器句柄
    uint32_t tim_channel;     // 定时器通道
} Servo;

void Servo_Init(Servo *servo, TIM_HandleTypeDef *htim, uint32_t tim_channel);
void Servo_SetAngle(Servo *servo, float angle);

#endif // SERVO_H
