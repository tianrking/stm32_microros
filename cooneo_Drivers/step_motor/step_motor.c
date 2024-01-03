#include "step_motor.h"


// change frequency for speed control
void Configure_TIM2_PWM_Frequency(uint32_t input)
{
    uint32_t prescaler = 1;  // Prescaler is 1
    uint32_t max_frequency = 13000; // 13kHz
    uint32_t min_frequency = 0; // 0Hz
    uint32_t clock_frequency = 72000000; // Assuming 84MHz Timer Clock frequency

    // Map input 0-100 to 0-13kHz
    uint32_t target_frequency = ((max_frequency - min_frequency) * input / 100) + min_frequency;

    // Calculate period based on the target frequency
    uint32_t period = (clock_frequency / ((prescaler + 1) * target_frequency)) - 1;

    /* Stop TIM4 to set the new configuration */
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_ALL);

    /* Set the new prescaler and period values */
    htim2.Init.Prescaler = prescaler - 1;  // Prescaler value in register is 1 less than actual value
    htim2.Init.Period = period;

    /* Re-initialize TIM4 with new settings */
    if(HAL_TIM_PWM_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    /* Start TIM4 with the new frequency settings */

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
}

//change for angle control


void rotate_motor_by_angle(float angle)
{
    // 计算需要的脉冲数
    int pulse_count = (int)(angle / STEP_ANGLE);

    // 产生PWM脉冲
    for(int i = 0; i < pulse_count; i++)
    {
        // 设置占空比，产生一个脉冲
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 500); // 500 out of 1000, 50% duty cycle

        // 简单延时以模拟脉冲宽度
        HAL_Delay(1); // 延时1ms，这个值取决于你的步进电机和驱动器的要求

        // 停止脉冲
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
        // 等待下一个脉冲
        HAL_Delay(1); // 根据步进电机的响应速度调整
    }
}