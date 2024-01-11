#include "include.h"

#define TIM_PERIOD 1000
#define MIN_PULSE_WIDTH 50/2  // 1ms pulse width at 50Hz
#define MAX_PULSE_WIDTH 250/2  // 2ms pulse width at 50Hz
#define SERVO_COUNT 3        // 总舵机数量

typedef struct
{
    TIM_HandleTypeDef *htim; // 定时器句柄
    uint32_t channel;        // PWM通道
    float angle;             // 当前角度
} Servo_se;

Servo_se servos[SERVO_COUNT] = {
    {&htim4, TIM_CHANNEL_3, 0.0f}, //PD12   //cooneo f4 servo1 PB8
    {&htim4, TIM_CHANNEL_4, 0.0f}, //PD13    //cooneo f4 servo1 PB9
    {&htim5, TIM_CHANNEL_3, 0.0f},                                         //A2
    // {&htim5, TIM_CHANNEL_4, 0.0f},                                         //A3
    // {&htim4, TIM_CHANNEL_3, 0.0f}  //PD14
};

void Servos_Init(void) {
    // 针对每个舵机的初始化...
    //HAL_TIM_PWM_Start(&htim14,TIM_CHANNEL_1);
    //HAL_TIM_PWM_Start(&htim14,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_3);
}

// void Set_Servo_Angle(Servo_se *servo, float angle) {
//     if (angle < 0.0f) angle = 0.0f;
//     if (angle > 180.0f) angle = 180.0f;
//     servo->angle = angle;

//     uint32_t pulse_width = (uint32_t)(MIN_PULSE_WIDTH + (angle / 180.0f) * (MAX_PULSE_WIDTH - MIN_PULSE_WIDTH));
//     __HAL_TIM_SET_COMPARE(servo->htim, servo->channel, pulse_width);
// }

void Set_Servo_Angle(uint8_t servo_id, float angle) {
    if (servo_id >= SERVO_COUNT) return; // 无效的ID
    // if (angle < 0.0f) angle = 0.0f;
    // if (angle > 180.0f) angle = 180.0f;

    if (angle < 0.0f) angle = 0.0f;
    if (angle > 60.0f) angle = 60.0f;
    
    Servo_se *servo = &servos[servo_id];
    servo->angle = angle;

    uint32_t pulse_width = (uint32_t)(MIN_PULSE_WIDTH + (angle / 180.0f) * (MAX_PULSE_WIDTH - MIN_PULSE_WIDTH));
    __HAL_TIM_SET_COMPARE(servo->htim, servo->channel, pulse_width);
}





