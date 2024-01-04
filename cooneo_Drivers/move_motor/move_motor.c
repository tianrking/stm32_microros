#include "move_motor.h"
#include "include.h"

typedef struct {
    TIM_HandleTypeDef *htim;  // PWM使用的定时器句柄
    uint32_t pwm_channel;     // PWM通道
    GPIO_TypeDef* gpio_port_direction1;  // 方向控制引脚1的GPIO端口
    uint16_t gpio_pin_direction1;        // 方向控制引脚1
    GPIO_TypeDef* gpio_port_direction2;  // 方向控制引脚2的GPIO端口
    uint16_t gpio_pin_direction2;        // 方向控制引脚2
    // 可以添加更多参数，如编码器引脚等
} MotorControl;


// PWMA PC6  TIM3->CCR1
// AIN1 PC0
// AIN2 PC1
// A15 B3

// PWMB PC7  TIM3->CCR2
// BIN1 PC2  
// BIN2 PC3
// A7 A6

// PWMC PC8
// CIN1 PB12
// CIN2 PB13
// B7 B6

// PWMD PC9
// DIN1 PB14
// DIN2 PB15
// A0 A1

// 假设有两个电机

MotorControl motors[] = {
    // 示例电机初始化，确保正确设置所有参数
    {&htim3, TIM_CHANNEL_1, GPIOC, GPIO_PIN_0, GPIOC, GPIO_PIN_1},  // 电机A
    {&htim3, TIM_CHANNEL_2, GPIOC, GPIO_PIN_2, GPIOC, GPIO_PIN_3},
    {&htim3, TIM_CHANNEL_3, GPIOB, GPIO_PIN_12, GPIOB, GPIO_PIN_13},
    {&htim3, TIM_CHANNEL_4, GPIOB, GPIO_PIN_14, GPIOB, GPIO_PIN_15},
    // ...根据需要添加更多电机
};

void control_motor_init(void){
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
    return ; 
}

void control_motor_speed(uint8_t motor_id, float speed) {
    if (motor_id >= sizeof(motors) / sizeof(motors[0])) {
        // 如果ID不合法，则返回或处理错误
        return;
    }
    
    // 获取对应电机的配置
    MotorControl* motor = &motors[motor_id];

    // 根据速度的正负设置GPIO控制方向
    if(speed >= 0) {
        HAL_GPIO_WritePin(motor->gpio_port_direction1, motor->gpio_pin_direction1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(motor->gpio_port_direction2, motor->gpio_pin_direction2, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(motor->gpio_port_direction1, motor->gpio_pin_direction1, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(motor->gpio_port_direction2, motor->gpio_pin_direction2, GPIO_PIN_SET);
        speed = -speed;  // 速度转为正值，用于下面设置PWM
    }

    // 根据速度绝对值设置PWM占空比（0.0到100.0之间）
    uint32_t pulse_length = (uint32_t)(((motor->htim->Init.Period + 1) * speed) / 100.0f);
    __HAL_TIM_SET_COMPARE(motor->htim, motor->pwm_channel, pulse_length);
}


volatile int32_t encoder_ticks = 0;
#define ENCODER_PIN_A 6
#define ENCODER_PIN_B 7
// EXTI LineX Interrupt Handler，替换X为你的外部中断编号
void EXTIx_IRQHandler(void) {
    // 确定是哪个引脚触发了中断，并处理编码器的A和B状态来确定方向
    // ...[处理编码器逻辑]...
    
    // encoder_ticks++;  // 或在某些条件下进行自增或自减

    // // 清除中断标志
    // __HAL_GPIO_EXTI_CLEAR_IT(ENCODER_PIN_A);  // 根据实际情况修改
    // __HAL_GPIO_EXTI_CLEAR_IT(ENCODER_PIN_B);  // 根据实际情况修改
}

int factor = 1;
float calculate_speed(uint32_t delta_ticks, uint32_t delta_time) {
    // need fix
    float speed = (float)delta_ticks / delta_time;  // 示例计算
    return speed;
}


void reset_encoder_ticks() {
    encoder_ticks = 0;
}

float motor_speed;
// void TIM14_IRQHandler(void) {
//     // static uint32_t last_ticks = 0;
//     // static uint32_t last_time = 0;
//     // if(__HAL_TIM_GET_FLAG(&htim14, TIM_FLAG_UPDATE) != RESET) {
//     //     if(__HAL_TIM_GET_IT_SOURCE(&htim14, TIM_IT_UPDATE) != RESET) {
//     //         __HAL_TIM_CLEAR_IT(&htim14, TIM_IT_UPDATE);

//     //         uint32_t current_time = HAL_GetTick();  // 获取当前时间
//     //         uint32_t delta_ticks = encoder_ticks - last_ticks;  // 自上次以来的脉冲数变化
//     //         uint32_t delta_time = current_time - last_time;  // 时间变化

//     //         // 计算转速，这里需要根据实际情况调整公式
//     //         motor_speed = calculate_speed(delta_ticks, delta_time);

//     //         // 更新上一次的记录
//     //         last_ticks = encoder_ticks;
//     //         last_time = current_time;
//     //     }
//     // }
// }


