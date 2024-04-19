// motor.c
#include "motor.h"

void Motor_Init(Motor *motor, TIM_HandleTypeDef *htim, uint32_t tim_channel,
                GPIO_TypeDef* port_dir1, uint16_t pin_dir1,
                GPIO_TypeDef* port_dir2, uint16_t pin_dir2) {
    motor->htim = htim;
    motor->tim_channel = tim_channel;
    motor->port_dir1 = port_dir1;
    motor->pin_dir1 = pin_dir1;
    motor->port_dir2 = port_dir2;
    motor->pin_dir2 = pin_dir2;

    // 初始化PWM
    HAL_TIM_PWM_Start(htim, tim_channel);

    // 初始化方向控制引脚
//    GPIO_InitTypeDef GPIO_InitStruct = {0};
//    GPIO_InitStruct.Pin = pin_dir1 | pin_dir2;
//    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//    HAL_GPIO_Init(port_dir1, &GPIO_InitStruct);  // 注意：如果两个方向引脚不在同一端口，需要分别初始化
}

void SetMotorSpeed(Motor *motor, uint32_t speed) {
    __HAL_TIM_SET_COMPARE(motor->htim, motor->tim_channel, speed);
}

void SetMotorDirection(Motor *motor, int direction) {
    HAL_GPIO_WritePin(motor->port_dir1, motor->pin_dir1, (direction > 0) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor->port_dir2, motor->pin_dir2, (direction < 0) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void SetMotorControl(Motor *motor, int32_t speed) {
    if (speed < 0) {
        HAL_GPIO_WritePin(motor->port_dir1, motor->pin_dir1, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(motor->port_dir2, motor->pin_dir2, GPIO_PIN_SET);
        speed = -speed;  // 速度取绝对值，因为PWM不能为负
    } else {
        HAL_GPIO_WritePin(motor->port_dir1, motor->pin_dir1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(motor->port_dir2, motor->pin_dir2, GPIO_PIN_RESET);
    }

    // 确保PWM值不超出范围
    uint32_t pwm_value = (uint32_t) (speed > 100 ? 100 : speed);
    __HAL_TIM_SET_COMPARE(motor->htim, motor->tim_channel, pwm_value);
}
