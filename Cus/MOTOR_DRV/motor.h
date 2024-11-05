// motor.h
#ifndef __MOTOR_H
#define __MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "tim.h"

// 电机方向枚举
typedef enum {
    MOTOR_FORWARD = 1,
    MOTOR_BACKWARD = -1,
    MOTOR_STOP = 0
} MotorDirection;

// 电机配置结构体
typedef struct {
    TIM_HandleTypeDef *htim;        // 定时器句柄
    uint32_t channel;               // PWM通道
    GPIO_TypeDef *dir_port1;        // 方向控制端口1
    uint16_t dir_pin1;              // 方向控制引脚1
    GPIO_TypeDef *dir_port2;        // 方向控制端口2
    uint16_t dir_pin2;              // 方向控制引脚2
    uint32_t current_speed;         // 当前速度值(0-100)
    MotorDirection current_dir;      // 当前方向
} Motor_HandleTypeDef;

// 电机初始化结构体
typedef struct {
    TIM_HandleTypeDef *htim;
    uint32_t channel;
    GPIO_TypeDef *dir_port1;
    uint16_t dir_pin1;
    GPIO_TypeDef *dir_port2;
    uint16_t dir_pin2;
} Motor_InitTypeDef;

// 函数声明
void Motor_Init(Motor_HandleTypeDef *hmotor, Motor_InitTypeDef *init);
void Motor_SetSpeed(Motor_HandleTypeDef *hmotor, int32_t speed);
void Motor_Stop(Motor_HandleTypeDef *hmotor);

// 预定义的电机配置
extern Motor_HandleTypeDef hmotor1;  // 电机1句柄
extern Motor_HandleTypeDef hmotor2;  // 电机2句柄

void Motors_Init(void);  // 初始化所有电机

#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_H */