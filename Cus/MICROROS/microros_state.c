#include "microros_state.h"
#include <string.h>

/* 静态状态管理实例 */
static struct {
    VehicleParams vehicle_params;
} state_manager;

/* 默认参数 */
static const VehicleParams DEFAULT_VEHICLE_PARAMS = {
    .type = VEHICLE_TYPE_DIFFERENTIAL,
    .wheelRadius = 0.065f,
    .vehicleWidth = 0.32f,
    .vehicleLength = 0.32f
};

/* 车辆类型字符串，用于调试 */
static const char* VEHICLE_TYPE_STRINGS[] = {
    "DIFFERENTIAL",
    "ACKERMANN",
    "MECANUM",
    "BOAT"
};

/* 初始化状态管理器 */
void MicroROSState_Init(void) {
    MicroROSState_SetDefaultVehicleParams();
}

/* PID参数相关函数实现 */
void MicroROSState_SetDefaultPIDParams(PID_HandleTypeDef *pid) {
    if (pid != NULL) {
        PID_Init(pid, 1.0f, 0.0f, 0.0f, 0.0f);  // 默认PID参数
        PID_SetOutputLimits(pid, -1000.0f, 1000.0f);
        PID_SetIntegralLimit(pid, 1000.0f);
    }
}

bool MicroROSState_UpdatePIDParams(PID_HandleTypeDef *pid, float kp, float ki, float kd) {
    if (pid == NULL || kp < 0 || ki < 0 || kd < 0) {
        return false;
    }
    
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    
    // 重置PID控制器状态
    PID_Reset(pid);
    
    return true;
}

bool MicroROSState_UpdatePIDLimits(PID_HandleTypeDef *pid, 
                                  float output_min, 
                                  float output_max, 
                                  float integral_limit) {
    if (pid == NULL || output_min >= output_max || integral_limit < 0) {
        return false;
    }
    
    PID_SetOutputLimits(pid, output_min, output_max);
    PID_SetIntegralLimit(pid, integral_limit);
    
    return true;
}

/* 车身参数相关函数实现 */
void MicroROSState_SetDefaultVehicleParams(void) {
    memcpy(&state_manager.vehicle_params, &DEFAULT_VEHICLE_PARAMS, sizeof(VehicleParams));
}

bool MicroROSState_SetVehicleParams(VehicleType type, float wheel_radius, float width, float length) {
    if (wheel_radius <= 0 || width <= 0 || length <= 0) {
        return false;
    }
    
    state_manager.vehicle_params.type = type;
    state_manager.vehicle_params.wheelRadius = wheel_radius;
    state_manager.vehicle_params.vehicleWidth = width;
    state_manager.vehicle_params.vehicleLength = length;
    
    return true;
}

const VehicleParams* MicroROSState_GetVehicleParams(void) {
    return &state_manager.vehicle_params;
}

/* 实用函数实现 */
const char* MicroROSState_GetVehicleTypeString(VehicleType type) {
    if (type >= VEHICLE_TYPE_DIFFERENTIAL && type <= VEHICLE_TYPE_BOAT) {
        return VEHICLE_TYPE_STRINGS[type];
    }
    return "UNKNOWN";
}