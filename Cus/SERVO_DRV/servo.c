// servo.c
#include "servo.h"

void Servo_Init(Servo *servo, TIM_HandleTypeDef *htim, uint32_t tim_channel) {
    servo->htim = htim;
    servo->tim_channel = tim_channel;
    HAL_TIM_PWM_Start(htim, tim_channel);  // 启动PWM
}

void Servo_SetAngle(Servo *servo, float angle) {
    if (angle < 0.0f) angle = 0.0f;  // 确保角度不小于0
    if (angle > 180.0f) angle = 180.0f;  // 确保角度不大于180

    // 将角度映射到脉宽（1ms到2ms对应于一个周期内的计数）
    // 假设TIM的周期设置为20000（20ms），则1ms对应1000，2ms对应2000
    uint32_t pulse = (uint32_t)((angle / 180.0f) * 1000.0f + 1000.0f);
    __HAL_TIM_SET_COMPARE(servo->htim, servo->tim_channel, pulse);
}
