#ifndef ROBOT_STATE_H
#define ROBOT_STATE_H

#include "main.h"

// 定义电机控制模式枚举
typedef enum {
    MOTOR_OPEN_LOOP,
    MOTOR_CLOSE_LOOP,
    MOTOR_ERROR,
    MOTOR_TEST
} MotorControlMode;

// 定义操作模式枚举
typedef enum {
    OP_MODE_UNSET,
    OP_MODE_UART,   // 串口控制模式
    OP_MODE_CAN,    // CAN总线模式
    OP_MODE_MICROROS,  // MicroROS模式
    OP_MODE_AT      // AT指令模式
} OperationMode;

// 定义轮子参数结构
typedef struct {
    float diameter;
    int pulses;
    float maxSpeed;
    float minSpeed;
    int exceptSpeed;
    int realSpeed;
    MotorControlMode controlMode; // 每个轮子的控制模式
} WheelParams;

// 定义机器人状态结构
typedef struct {
    OperationMode operationMode; // 全局操作模式
    WheelParams leftWheel;
    WheelParams rightWheel;
    int servoAngle;
    int realAngle;
} RobotState;

// 函数声明
void RobotState_Init(void);
void RobotState_SetOperationMode(OperationMode newMode);
void RobotState_SetWheelControlMode(char wheel, MotorControlMode mode);
void RobotState_SetWheelParams(char wheel, WheelParams params);
void RobotState_SetWheelSpeed(char wheel, int speed);
void RobotState_SyncWheelSpeed_real(char wheel, int speed);

void RobotState_SetServoAngle(int angle);

RobotState* RobotState_Get(void);

#endif /* ROBOT_STATE_H */
