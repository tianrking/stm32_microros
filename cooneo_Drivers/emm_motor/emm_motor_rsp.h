#ifndef MY_MOTOR_CONTROL_H
#define MY_MOTOR_CONTROL_H

#include "stm32f4xx_hal.h"  // 或者适用于你的MCU系列的相应头文件
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "include.h"

#define MAX_DATA_LENGTH 10  
// 错误代码定义
#define ERR_INVALID_DATA -1
#define ERR_UNKNOWN_CMD -2
#define ERR_BUFFER_OVERFLOW -3
// ... 其他错误代码

void parseMotorRealTimePosition(uint8_t* data);
void parseEncoderValue(uint8_t* data);
void parseInputPulses(uint8_t* data);
void parsePositionError(uint8_t* data);
void parseEnableStatus(uint8_t* data);
void parseStallFlag(uint8_t* data);

#endif // MY_MOTOR_CONTROL_H
