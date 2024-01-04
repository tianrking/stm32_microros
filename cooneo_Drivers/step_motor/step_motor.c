// #include "step_motor.h"
// #include "include.h"

// typedef struct {
//     TIM_HandleTypeDef *htim;  // 定时器句柄
//     uint32_t channel;         // PWM通道
//     float step_angle;         // 电机单步角度
// } StepMotor_angle_control;

// // 假设定义一些电机实例
// StepMotor_angle_control motors_ac[] = {
//     {&htim2, TIM_CHANNEL_3, 1.8f},  // 电机1
//     {&htim2, TIM_CHANNEL_4, 1.8f},  // 电机2
//     // ... 根据需要可以定义更多电机
// };

// void step_motor_angle_control(uint8_t motor_id, float angle) {
//     // if (motor_id >= sizeof(motors_ac) / sizeof(motors_ac[0])) {
//     //     // 如果ID不合法，则返回或者处理错误
//     //     return;
//     // }

//     // 获取对应电机的配置
//     StepMotor_angle_control *motor = &motors_ac[motor_id];

//     // 计算需要的脉冲数
//     int pulse_count = (int)(angle / motor->step_angle);
//     HAL_TIM_PWM_Start(motor->htim, motor->channel);
//     // 产生PWM脉冲
//     for(int i = 0; i < pulse_count; i++) {
//         // 设置占空比，产生一个脉冲
//         __HAL_TIM_SET_COMPARE(motor->htim, motor->channel, 500); // 50% duty cycle

//         // 简单延时以模拟脉冲宽度
//         HAL_Delay(1); // 延时1ms

//         // 停止脉冲
//         __HAL_TIM_SET_COMPARE(motor->htim, motor->channel, 0);
        
//         // 等待下一个脉冲
//         HAL_Delay(1);
//     }
//     HAL_TIM_PWM_Stop(motor->htim, motor->channel);
// }


// // change frequency for speed control
// void Configure_TIM2_PWM_Frequency(uint32_t input)
// {
//     uint32_t prescaler = 1;  // Prescaler is 1
//     uint32_t max_frequency = 13000; // 13kHz
//     uint32_t min_frequency = 0; // 0Hz
//     uint32_t clock_frequency = 72000000; // Assuming 84MHz Timer Clock frequency

//     // Map input 0-100 to 0-13kHz
//     uint32_t target_frequency = ((max_frequency - min_frequency) * input / 100) + min_frequency;

//     // Calculate period based on the target frequency
//     uint32_t period = (clock_frequency / ((prescaler + 1) * target_frequency)) - 1;

//     /* Stop TIM4 to set the new configuration */
//     HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_ALL);

//     /* Set the new prescaler and period values */
//     htim2.Init.Prescaler = prescaler - 1;  // Prescaler value in register is 1 less than actual value
//     htim2.Init.Period = period;

//     /* Re-initialize TIM4 with new settings */
//     if(HAL_TIM_PWM_Init(&htim2) != HAL_OK)
//     {
//         Error_Handler();
//     }

//     /* Start TIM4 with the new frequency settings */

//     HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
//     HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
// }