// encoder.c
#include "encoder.h"
#include "tim.h"

Encoder_HandleTypeDef hencoder1;
Encoder_HandleTypeDef hencoder2;

static void Encoder_Init(Encoder_HandleTypeDef *hencoder, 
                        TIM_HandleTypeDef *htim_encoder, 
                        float counts_per_revolution)
{
    hencoder->htim_encoder = htim_encoder;
    hencoder->counts_per_revolution = counts_per_revolution;
    hencoder->last_pulse_count = 0;
    hencoder->speed_rpm = 0.0f;
    hencoder->speed_rps = 0.0f;
    
    // 启动编码器
    HAL_TIM_Encoder_Start(htim_encoder, TIM_CHANNEL_ALL);
    __HAL_TIM_SET_COUNTER(htim_encoder, 0);
}

void Encoders_Init(void)
{
    // 初始化两个编码器
    Encoder_Init(&hencoder1, &htim5, 1320.0f);  // TIM5
    Encoder_Init(&hencoder2, &htim3, 1320.0f);  // TIM3
    
    // 启动TIM12作为1秒定时器
    HAL_TIM_Base_Start_IT(&htim12);
}

void Encoder_Update(Encoder_HandleTypeDef *hencoder)
{
    // 读取当前计数值
    int32_t current_count = (int16_t)__HAL_TIM_GET_COUNTER(hencoder->htim_encoder);
    
    // 计算脉冲差值
    int32_t pulse_diff = current_count - hencoder->last_pulse_count;
    
    // 处理计数器溢出/下溢
    if (pulse_diff > 32767) {
        pulse_diff -= 65536;
    } else if (pulse_diff < -32768) {
        pulse_diff += 65536;
    }
    
    // 计算RPS (每秒转数)
    // RPS = pulse_diff / counts_per_revolution
    float rps = (float)pulse_diff / hencoder->counts_per_revolution;
    
    // 计算RPM
    float rpm = rps * 60.0f;
    
    // 更新速度值
    hencoder->speed_rps = rps;
    hencoder->speed_rpm = rpm;
    
    // 保存当前计数值
    hencoder->last_pulse_count = current_count;
}

float Encoder_GetSpeedRPM(Encoder_HandleTypeDef *hencoder)
{
    return hencoder->speed_rpm;
}

float Encoder_GetSpeedRPS(Encoder_HandleTypeDef *hencoder)
{
    return hencoder->speed_rps;
}

int32_t Encoder_GetCount(Encoder_HandleTypeDef *hencoder)
{
    return (int16_t)__HAL_TIM_GET_COUNTER(hencoder->htim_encoder);
}

void Encoder_ResetCount(Encoder_HandleTypeDef *hencoder)
{
    __HAL_TIM_SET_COUNTER(hencoder->htim_encoder, 0);
    hencoder->last_pulse_count = 0;
}

// 在你的定时器中断处理函数中添加这个
// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// {
//     if (htim->Instance == TIM12)
//     {
//         // 每秒更新一次速度
//         Encoder_Update(&hencoder1);
//         Encoder_Update(&hencoder2);
//     }
// }