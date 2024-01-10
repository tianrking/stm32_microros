#include "step_motor.h"
#include "include.h"

// typedef struct {
//     TIM_HandleTypeDef *htim;  // 定时器句柄
//     uint32_t channel;         // PWM通道
//     float step_angle;         // 电机单步角度
// } StepMotor_angle_control;

// // 假设定义一些电机实例
// StepMotor_angle_control motors_ac[] = {
//     {&htim9, TIM_CHANNEL_1, 1.8f / 16},  // 电机1  PE5 
//     {&htim9, TIM_CHANNEL_2, 1.8f / 16},  // 电机2  PE6
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
//         __HAL_TIM_SET_COMPARE(motor->htim, motor->channel, 20); // 2 /50 duty cycle  20khz

//         // 简单延时以模拟脉冲宽度
//         HAL_Delay(1); // 延时1ms

//         // 停止脉冲
//         __HAL_TIM_SET_COMPARE(motor->htim, motor->channel, 0);
        
//         // 等待下一个脉冲
//         HAL_Delay(1);
//     }
//     HAL_TIM_PWM_Stop(motor->htim, motor->channel);
// }


// change frequency for speed control
void Configure_TIM9_PWM_Frequency(uint32_t input)
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
    HAL_TIM_PWM_Stop(&htim9, TIM_CHANNEL_ALL);

    /* Set the new prescaler and period values */
    htim9.Init.Prescaler = prescaler - 1;  // Prescaler value in register is 1 less than actual value
    htim9.Init.Period = period;

    /* Re-initialize TIM4 with new settings */
    if(HAL_TIM_PWM_Init(&htim9) != HAL_OK)
    {
        Error_Handler();
    }

    /* Start TIM4 with the new frequency settings */

    HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_2);
}


typedef struct {
    TIM_HandleTypeDef *htim;   // 定时器句柄
    uint32_t channel;          // PWM通道
    float step_angle;          // 电机单步角度
    uint32_t pulses;           // 需要产生的脉冲数
} StepMotor;

StepMotor motors_sc[] = {
    {&htim9, TIM_CHANNEL_1, 1.8f / 16, 0},  // 电机1
    {&htim9, TIM_CHANNEL_2, 1.8f / 16, 0},  // 电机2
    // ... 根据需要可以定义更多电机
};

volatile int pulse_count = 0;
int target_pulses = 10 ;

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM9) {
        for (int i = 0; i < sizeof(motors_sc) / sizeof(StepMotor); ++i) {
            if ((htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1 && motors_sc[i].channel == TIM_CHANNEL_1) ||
                (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2 && motors_sc[i].channel == TIM_CHANNEL_2) ||
                (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3 && motors_sc[i].channel == TIM_CHANNEL_3) ||
                (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4 && motors_sc[i].channel == TIM_CHANNEL_4)) {
                if (motors_sc[i].pulses > 0) {
                    motors_sc[i].pulses--;
                    if (motors_sc[i].pulses == 0) {
                        HAL_TIM_PWM_Stop_IT(motors_sc[i].htim, motors_sc[i].channel);
                    }
                }
            }
        }
    }
}

void StartMotorPWM(uint8_t motor_id, float angle) {
    if (motor_id >= sizeof(motors_sc) / sizeof(motors_sc[0])) {
        return; // 检查电机ID是否有效
    }

    StepMotor *motor = &motors_sc[motor_id];
    motor->pulses = (uint32_t)(angle / motor->step_angle); // 计算脉冲数

    TIM9->CCR1 = 20;
    TIM9->CCR2 = 20;
    // TIM9->CCR1 = 20;

    if (motor->pulses > 0) {
        HAL_TIM_PWM_Start_IT(motor->htim, motor->channel); // 启动PWM并开启中断
    }
}