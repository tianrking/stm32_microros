// pid.c
#include "pid.h"
#include <math.h>

void PID_Init(PID_HandleTypeDef *pid, float Kp, float Ki, float Kd, float setpoint)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->setpoint = setpoint;
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
    pid->output = 0.0f;
    
    // 默认输出限幅为PWM范围
    pid->output_limit_min = -1000.0f;
    pid->output_limit_max = 1000.0f;
    
    // 默认积分限幅
    pid->integral_limit = 1000.0f;
}

float PID_Update(PID_HandleTypeDef *pid, float measurement)
{
    float error = pid->setpoint - measurement;
    
    // 计算积分项
    pid->integral += error;
    
    // 积分限幅
    if (pid->integral > pid->integral_limit)
        pid->integral = pid->integral_limit;
    else if (pid->integral < -pid->integral_limit)
        pid->integral = -pid->integral_limit;
    
    // 计算微分项
    float derivative = error - pid->last_error;
    
    // 计算PID输出
    pid->output = pid->Kp * error + 
                 pid->Ki * pid->integral + 
                 pid->Kd * derivative;
    
    // 输出限幅
    if (pid->output > pid->output_limit_max)
        pid->output = pid->output_limit_max;
    else if (pid->output < pid->output_limit_min)
        pid->output = pid->output_limit_min;
    
    // 保存误差
    pid->last_error = error;
    
    return pid->output;
}

void PID_SetSetpoint(PID_HandleTypeDef *pid, float setpoint)
{
    pid->setpoint = setpoint;
}

void PID_Reset(PID_HandleTypeDef *pid)
{
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
    pid->output = 0.0f;
}

void PID_SetOutputLimits(PID_HandleTypeDef *pid, float min, float max)
{
    pid->output_limit_min = min;
    pid->output_limit_max = max;
}

void PID_SetIntegralLimit(PID_HandleTypeDef *pid, float limit)
{
    pid->integral_limit = limit;
}