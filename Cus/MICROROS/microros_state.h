#ifndef MICROROS_STATE_H
#define MICROROS_STATE_H

#include <stdint.h>
#include <stdbool.h>
#include "pid.h"  // 包含PID控制器头文件

/* Vehicle types */
typedef enum {
    VEHICLE_TYPE_DIFFERENTIAL,
    VEHICLE_TYPE_ACKERMANN,
    VEHICLE_TYPE_MECANUM,
    VEHICLE_TYPE_BOAT
} VehicleType;

/* Vehicle parameters structure */
typedef struct {
    VehicleType type;
    float wheelRadius;  // 轮子半径(m)
    float vehicleWidth;  // 车身宽度(m)
    float vehicleLength;  // 车身长度(m)
} VehicleParams;

/* 状态管理器初始化 */
void MicroROSState_Init(void);

/* PID参数相关函数 */
void MicroROSState_SetDefaultPIDParams(PID_HandleTypeDef *pid);
bool MicroROSState_UpdatePIDParams(PID_HandleTypeDef *pid, float kp, float ki, float kd);
bool MicroROSState_UpdatePIDLimits(PID_HandleTypeDef *pid, float output_min, float output_max, float integral_limit);

/* 车身参数相关函数 */
void MicroROSState_SetDefaultVehicleParams(void);
bool MicroROSState_SetVehicleParams(VehicleType type, float wheel_radius, float width, float length);
const VehicleParams* MicroROSState_GetVehicleParams(void);

/* 实用函数 */
const char* MicroROSState_GetVehicleTypeString(VehicleType type);

#endif /* MICROROS_STATE_H */