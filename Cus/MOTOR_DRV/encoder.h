// encoder.h
#ifndef ENCODER_H
#define ENCODER_H

#include "stm32f4xx_hal.h"

typedef struct {
    TIM_HandleTypeDef *htim_encoder;    // 编码器定时器句柄
    float counts_per_revolution;         // 每圈的编码器计数
    volatile int32_t last_pulse_count;   // 上次的脉冲计数
    volatile float speed_rpm;            // 速度 (RPM)
    volatile float speed_rps;            // 速度 (RPS - 每秒转数)
} Encoder_HandleTypeDef;

extern Encoder_HandleTypeDef hencoder1;  // 电机1编码器
extern Encoder_HandleTypeDef hencoder2;  // 电机2编码器

void Encoders_Init(void);
void Encoder_Update(Encoder_HandleTypeDef *hencoder);
float Encoder_GetSpeedRPM(Encoder_HandleTypeDef *hencoder);
float Encoder_GetSpeedRPS(Encoder_HandleTypeDef *hencoder);
int32_t Encoder_GetCount(Encoder_HandleTypeDef *hencoder);
void Encoder_ResetCount(Encoder_HandleTypeDef *hencoder);

#endif // ENCODER_H