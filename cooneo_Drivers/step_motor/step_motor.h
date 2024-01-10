#ifndef __STEP_MOTOR_H__
#define __STEP_MOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "include.h"

// 假设步进电机步进角为1.8度，每圈需要200个脉冲
#define STEP_ANGLE 1.8
#define STEPS_PER_REV 200

void step_motor_speed_control(uint8_t motor_id, uint32_t speed_percent);
void step_motor_angle_control(uint8_t motor_id, float angle) ;
size_t get_sc_motor_count(void);
size_t get_ac_motor_count(void); 


// test func
void Configure_TIM9_PWM_Frequency(uint32_t input);
void step_motor_angle_control_test(float angle);

// 定义一个结构体来存储电机的目标角度
typedef struct {
    float angle;  // 目标角度
    bool new_data_ready;  // 新数据是否准备好
} MotorTarget;

void motor_control_callback(const void *msgin);
void StartMotorPWM(uint8_t motor_id, float angle) ;
// void StartPWM();
#endif