// motor.c
#include "motor.h"

// 全局电机句柄定义
Motor_HandleTypeDef hmotor1;
Motor_HandleTypeDef hmotor2;

// 内部函数声明
static void Motor_SetDirection(Motor_HandleTypeDef *hmotor, MotorDirection dir);

void Motor_Init(Motor_HandleTypeDef *hmotor, Motor_InitTypeDef *init)
{
    // 配置电机参数
    hmotor->htim = init->htim;
    hmotor->channel = init->channel;
    hmotor->dir_port1 = init->dir_port1;
    hmotor->dir_pin1 = init->dir_pin1;
    hmotor->dir_port2 = init->dir_port2;
    hmotor->dir_pin2 = init->dir_pin2;
    
    // 初始化默认状态
    hmotor->current_speed = 0;
    hmotor->current_dir = MOTOR_STOP;
    
    // 启动PWM
    HAL_TIM_PWM_Start(hmotor->htim, hmotor->channel);
    
    // 确保电机初始状态为停止
    Motor_Stop(hmotor);
}

void Motors_Init(void)
{
    // 电机1初始化配置 (PD8/PD9方向, PD12 PWM-TIM4_CH1)
    Motor_InitTypeDef motor1_init = {
        .htim = &htim1,
        .channel = TIM_CHANNEL_1,
        .dir_port1 = GPIOE,
        .dir_pin1 = GPIO_PIN_7,
        .dir_port2 = GPIOB,
        .dir_pin2 = GPIO_PIN_1
    };
    
    // 电机2初始化配置 (/PD11方向, PD13 PWM-TIM4_CH2)
    Motor_InitTypeDef motor2_init = {
        .htim = &htim1,
        .channel = TIM_CHANNEL_2,
        .dir_port1 = GPIOE,
        .dir_pin1 = GPIO_PIN_8,
        .dir_port2 = GPIOE,
        .dir_pin2 = GPIO_PIN_10
    };
    
    Motor_Init(&hmotor1, &motor1_init);
    Motor_Init(&hmotor2, &motor2_init);
}

void Motor_SetSpeed(Motor_HandleTypeDef *hmotor, int32_t speed)
{
    // 确定方向并获取绝对值
    MotorDirection dir;
    uint32_t abs_speed;
    
    if (speed > 0) {
        dir = MOTOR_FORWARD;
        abs_speed = (uint32_t)speed;
    } else if (speed < 0) {
        dir = MOTOR_BACKWARD;
        abs_speed = (uint32_t)(-speed);
    } else {
        dir = MOTOR_STOP;
        abs_speed = 0;
    }
    
    // 限制最大值为1000
    if (abs_speed > 1000) {
        abs_speed = 1000;
    }
    
    // 设置方向
    Motor_SetDirection(hmotor, dir);
    
    // 直接设置PWM值
    __HAL_TIM_SET_COMPARE(hmotor->htim, hmotor->channel, abs_speed);
    
    // 更新当前状态
    hmotor->current_speed = abs_speed;
    hmotor->current_dir = dir;
}

void Motor_Stop(Motor_HandleTypeDef *hmotor)
{
    Motor_SetDirection(hmotor, MOTOR_STOP);
    __HAL_TIM_SET_COMPARE(hmotor->htim, hmotor->channel, 0);
    hmotor->current_speed = 0;
    hmotor->current_dir = MOTOR_STOP;
}

// 内部函数实现
static void Motor_SetDirection(Motor_HandleTypeDef *hmotor, MotorDirection dir) {
    switch (dir) {
        case MOTOR_FORWARD:
            // 正向：1,0
            HAL_GPIO_WritePin(hmotor->dir_port1, hmotor->dir_pin1, GPIO_PIN_SET);    // 1
            HAL_GPIO_WritePin(hmotor->dir_port2, hmotor->dir_pin2, GPIO_PIN_RESET);  // 0
            break;
        case MOTOR_BACKWARD:
            // 反向：0,1
            HAL_GPIO_WritePin(hmotor->dir_port1, hmotor->dir_pin1, GPIO_PIN_RESET);  // 0
            HAL_GPIO_WritePin(hmotor->dir_port2, hmotor->dir_pin2, GPIO_PIN_SET);    // 1
            break;
        case MOTOR_STOP:
        default:
            // 停止：0,0
            HAL_GPIO_WritePin(hmotor->dir_port1, hmotor->dir_pin1, GPIO_PIN_RESET);  // 0
            HAL_GPIO_WritePin(hmotor->dir_port2, hmotor->dir_pin2, GPIO_PIN_RESET);  // 0
            break;
    }
}