// pid.h
#ifndef PID_H
#define PID_H

#include "stm32f4xx_hal.h"

typedef struct {
    float Kp;                  // 比例系数
    float Ki;                  // 积分系数
    float Kd;                  // 微分系数
    float setpoint;            // 目标值
    float integral;            // 积分项
    float last_error;          // 上次误差
    float output;             // PID输出
    float output_limit_min;    // 输出下限
    float output_limit_max;    // 输出上限
    float integral_limit;      // 积分限幅
} PID_HandleTypeDef;

// 函数声明
void PID_Init(PID_HandleTypeDef *pid, float Kp, float Ki, float Kd, float setpoint);
float PID_Update(PID_HandleTypeDef *pid, float measurement);
void PID_SetSetpoint(PID_HandleTypeDef *pid, float setpoint);
void PID_Reset(PID_HandleTypeDef *pid);
void PID_SetOutputLimits(PID_HandleTypeDef *pid, float min, float max);
void PID_SetIntegralLimit(PID_HandleTypeDef *pid, float limit);

#endif // PID_H