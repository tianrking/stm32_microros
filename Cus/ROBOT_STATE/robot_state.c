#include "robot_state.h"
#include <stdio.h>

RobotState robotState;

void RobotState_Init(void) {
    // 初始化操作模式为 AT 指令模式
    robotState.operationMode = OP_MODE_UNSET;

    // 初始化左右轮子参数和控制模式
    robotState.leftWheel = (WheelParams){.diameter = 0.0, .pulses = 0, .maxSpeed = 0.0, .minSpeed = 0.0, .exceptSpeed = 0, .controlMode = MOTOR_OPEN_LOOP};
    robotState.rightWheel = (WheelParams){.diameter = 0.0, .pulses = 0, .maxSpeed = 0.0, .minSpeed = 0.0, .exceptSpeed = 0, .controlMode = MOTOR_OPEN_LOOP};
}

void RobotState_SetOperationMode(OperationMode newMode) {
    robotState.operationMode = newMode;
    printf("Operation mode set to %d\n", newMode);
}

void RobotState_SetWheelControlMode(char wheel, MotorControlMode mode) {
    if (wheel == 'L') {
        robotState.leftWheel.controlMode = mode;
        printf("Left wheel control mode set to %d\n", mode);
    } else if (wheel == 'R') {
        robotState.rightWheel.controlMode = mode;
        printf("Right wheel control mode set to %d\n", mode);
    } else {
        printf("Error: Invalid wheel identifier\n");
    }
}

void RobotState_SetWheelParams(char wheel, WheelParams params) {
    if (wheel == 'L') {
        robotState.leftWheel = params;
        printf("Left wheel parameters updated\n");
    } else if (wheel == 'R') {
        robotState.rightWheel = params;
        printf("Right wheel parameters updated\n");
    } else {
        printf("Error: Invalid wheel identifier\n");
    }
}

void RobotState_SetWheelSpeed(char wheel, int speed) {
    if (wheel == 'L') {
        robotState.leftWheel.exceptSpeed = speed;
        printf("Left wheel speed set to %d\n", speed);
    } else if (wheel == 'R') {
        robotState.rightWheel.exceptSpeed = speed;
        printf("Right wheel speed set to %d\n", speed);
    } else {
        printf("Error: Invalid wheel identifier\n");
    }
}

void RobotState_SyncWheelSpeed_real(char wheel, int speed) {
    if (wheel == 'L') {
        robotState.leftWheel.realSpeed = speed;
        printf("Left wheel speed set to %d\n", speed);
    } else if (wheel == 'R') {
        robotState.rightWheel.realSpeed = speed;
        printf("Right wheel speed set to %d\n", speed);
    } else {
        printf("Error: Invalid wheel identifier\n");
    }
}

void RobotState_SetServoAngle(int angle){
    robotState.servoAngle = angle;
}

RobotState* RobotState_Get(void) {
    return &robotState;
}
