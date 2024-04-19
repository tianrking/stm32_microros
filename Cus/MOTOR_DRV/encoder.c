// encoder.c
#include "encoder.h"

void Encoder_Init(Encoder *encoder, TIM_HandleTypeDef *htim_encoder) {
    encoder->htim_encoder = htim_encoder;
    encoder->angular_speed = 0.0f;
    HAL_TIM_Encoder_Start(htim_encoder, TIM_CHANNEL_ALL); 
}

void Encoder_UpdateSpeed(Encoder *encoder) {

    int32_t current_pulse_count = __HAL_TIM_GET_COUNTER(encoder->htim_encoder);
    int32_t pulse_diff = current_pulse_count - encoder->last_pulse_count;

    // ?????????????
    if (pulse_diff < -32768) {  
        pulse_diff += 65536; 
    } else if (pulse_diff > 32767) {  
        pulse_diff -= 65536;  
    }

    encoder->angular_speed = pulse_diff;

    encoder->last_pulse_count = current_pulse_count;
    //    int32_t pulse_count = __HAL_TIM_GET_COUNTER(encoder->htim_encoder);

//  
//    if (pulse_count > 32767) {  
//        pulse_count -= 65535;
//    }

//    encoder->angular_speed = pulse_count;  


    //__HAL_TIM_SET_COUNTER(encoder->htim_encoder, 0);
}

float Encoder_GetAngularSpeed(Encoder *encoder) {
    return encoder->angular_speed;
}
