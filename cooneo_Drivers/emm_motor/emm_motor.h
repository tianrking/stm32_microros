#ifndef __EMM_MOTOR_H__
#define __EMM_MOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "include.h"
// 触发部分
void emm_TriggerEncoderCalibration(uint8_t motorId);
void emm_SetCurrentPositionToZero(uint8_t motorId);
void emmReleaseStallProtection(uint8_t motorId);

// 读取参数部分
void emm_ReadEncoderValue(uint8_t motorId);
void emm_ReadInputPulses(uint8_t motorId);
void emm_ReadMotorRealTimePosition(uint8_t motorId);
void emm_ReadPositionError(uint8_t motorId);
void emm_ReadEnableStatus(uint8_t motorId);
void emm_ReadStallFlag(uint8_t motorId);
void emm_ReadAutoHomeStatus(uint8_t motorId);

// 控制部分
void emm_ControlMotorEnable(uint8_t motorId, uint8_t enableStatus);
void emm_ControlMotorDirectionSpeed(uint8_t motorId, uint16_t directionSpeed, uint8_t acceleration);
void emm_StoreOrClearParameters(uint8_t motorId, uint8_t operation);
void emm_ControlMotorRelativeAngle(uint8_t motorId, uint16_t directionSpeed, uint8_t acceleration, uint32_t pulseCount);

uint16_t convertToDirectionSpeed(int speed) ;

//位置记录
void emm_UpdateMotorAngle(uint8_t motorId, double additionalAngle);

#endif