// motor.h
#ifndef MOTOR_H
#define MOTOR_H

#include "stm32f4xx_hal.h"

typedef struct {
    TIM_HandleTypeDef *htim;  // 定时器句柄
    uint32_t tim_channel;     // 定时器通道
    GPIO_TypeDef* port_dir1;  // 方向控制引脚1的GPIO端口
    uint16_t pin_dir1;        // 方向控制引脚1
    GPIO_TypeDef* port_dir2;  // 方向控制引脚2的GPIO端口
    uint16_t pin_dir2;        // 方向控制引脚2
} Motor;

void Motor_Init(Motor *motor, TIM_HandleTypeDef *htim, uint32_t tim_channel,
                GPIO_TypeDef* port_dir1, uint16_t pin_dir1,
                GPIO_TypeDef* port_dir2, uint16_t pin_dir2);

void SetMotorSpeed(Motor *motor, uint32_t speed);
void SetMotorDirection(Motor *motor, int direction);
void SetMotorControl(Motor *motor, int32_t speed);
#endif // MOTOR_H
