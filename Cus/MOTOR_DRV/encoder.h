// encoder.h
#ifndef ENCODER_H
#define ENCODER_H

#include "stm32f4xx_hal.h"

typedef struct {
    TIM_HandleTypeDef *htim_encoder; 
    volatile float angular_speed;   
    volatile int32_t last_pulse_count;
} Encoder;
extern Encoder myEncoder;
void Encoder_Init(Encoder *encoder, TIM_HandleTypeDef *htim_encoder);
void Encoder_UpdateSpeed(Encoder *encoder);
float Encoder_GetAngularSpeed(Encoder *encoder);

#endif // ENCODER_H
