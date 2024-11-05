// motor.h
#ifndef __MOTOR_H
#define __MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "tim.h"
#include "pid.h"
#include "encoder.h"

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
    uint32_t current_speed;         // 当前速度值(0-1000)
    MotorDirection current_dir;      // 当前方向
    // 增加速度控制相关字段
    PID_HandleTypeDef pid;          // PID控制器
    Encoder_HandleTypeDef *encoder; // 编码器
    float target_rpm;               // 目标转速
    float current_rpm;              // 当前实际转速
} Motor_HandleTypeDef;

// 电机初始化结构体
typedef struct {
    TIM_HandleTypeDef *htim;
    uint32_t channel;
    GPIO_TypeDef *dir_port1;
    uint16_t dir_pin1;
    GPIO_TypeDef *dir_port2;
    uint16_t dir_pin2;
    Encoder_HandleTypeDef *encoder; // 添加编码器
} Motor_InitTypeDef;

// 函数声明
void Motor_Init(Motor_HandleTypeDef *hmotor, Motor_InitTypeDef *init);
void Motor_SetSpeed(Motor_HandleTypeDef *hmotor, int32_t speed);
void Motor_Stop(Motor_HandleTypeDef *hmotor);
void Motor_SetTargetSpeed(Motor_HandleTypeDef *hmotor, float target_rpm);
void Motor_UpdateSpeed(Motor_HandleTypeDef *hmotor);

// 预定义的电机配置
extern Motor_HandleTypeDef hmotor1;  // 电机1句柄
extern Motor_HandleTypeDef hmotor2;  // 电机2句柄

void Motors_Init(void);  // 初始化所有电机

#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_H */