#include "AT.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include "robot_state.h"


void handleOperationModeCommand(const char *command);
void handleWheelCommand(const char *command);
void handleSetSpeedCommand(const char *command);
void handleGetWheelCommand(const char *command);
void handleGetOperationModeCommand(const char *command);
void handleSetWheelModeCommand(const char *command);
void handleResetCommand(const char *command);
void handleRebootCommand(const char *command); //system level

typedef struct {
    const char *commandPrefix;
    void (*handler)(const char*);
} CommandHandler;


CommandHandler commandHandlers[] = {
    {"AT+OPMODE=", handleOperationModeCommand},
    {"AT+SETWHEEL=", handleWheelCommand},
    {"AT+SETWHEELMODE=", handleSetWheelModeCommand}, 
    {"AT+SETSPEED=", handleSetSpeedCommand},
    {"AT+GETWHEEL=", handleGetWheelCommand},
    {"AT+GETOPMODE", handleGetOperationModeCommand},
    {"AT+RESET", handleResetCommand},
    {"AT+REBOOT", handleRebootCommand}, 
    {NULL, NULL}  
};

void process_at_command(char *command) {
    for (int i = 0; commandHandlers[i].commandPrefix != NULL; i++) {
        if (strncmp(command, commandHandlers[i].commandPrefix, strlen(commandHandlers[i].commandPrefix)) == 0) {
            commandHandlers[i].handler(command);
            return;
        }
    }
    char data[50];
    sprintf(data, "ERROR: Unknown command\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), 1000);
    if(RobotState_Get()->operationMode == OP_MODE_AT)
        HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), 1000);
}

void handleOperationModeCommand(const char *command) {
    char data[100];  // 调整缓冲区大小
    int commandOffset = 10; // 假设操作模式从第10个字符开始
    const char *modeCommand = command + commandOffset;
    RobotState *currentState = RobotState_Get(); // 获取当前状态一次
    int isExitingATMode = (currentState->operationMode == OP_MODE_AT && strcmp(modeCommand, "AT") != 0);

    // 确定要发送的消息
    if (strcmp(modeCommand, "UART") == 0) {
        sprintf(data, "OK: Changed from AT to UART mode\r\n");
        RobotState_SetOperationMode(OP_MODE_UART);
    } else if (strcmp(modeCommand, "CAN") == 0) {
        sprintf(data, "OK: Changed from AT to CAN mode\r\n");
        RobotState_SetOperationMode(OP_MODE_CAN);
    } else if (strcmp(modeCommand, "MICROROS") == 0) {
        sprintf(data, "OK: Changed from AT to MicroROS mode And now Reboot\r\n");
        HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), 1000);
        // HAL_Delay(500);  // 确保消息发送完成
        // NVIC_SystemReset();  // 触发系统重启
        // return;  // 确保不会继续执行其他代码
        RobotState_SetOperationMode(OP_MODE_MICROROS);
    } else if (strcmp(modeCommand, "AT") == 0) {
        sprintf(data, "OK: AT command mode set\r\n");
        // No mode change if already in AT mode
        HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), 1000);
        if(currentState->operationMode != OP_MODE_AT) {
            RobotState_SetOperationMode(OP_MODE_AT);
        }
    } else {
        sprintf(data, "ERROR: Invalid operation mode\r\n");
    }

    // 发送模式改变信息，仅当从AT模式退出时发送
    if (isExitingATMode) {
        HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), 1000);
    }

    // 总是在USART2上发送操作结果，用于调试或记录
    HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), 1000);
}



void handleWheelCommand(const char *command) {
    char data[200];
    char wheel;
    float diameter;
    int pulses;
    float maxSpeed;
    float minSpeed;
    MotorControlMode mode;

    if (sscanf(command + 12, "%c,%f,%d,%f,%f,%d", &wheel, &diameter, &pulses, &maxSpeed, &minSpeed, (int*)&mode) == 6) {
        WheelParams params = {diameter, pulses, maxSpeed, minSpeed, 0, mode};
        RobotState_SetWheelParams(wheel, params);
        sprintf(data, "OK: %c wheel set with mode %d\r\n", wheel, mode);
    } else {
        sprintf(data, "ERROR: Invalid wheel parameters\r\n");
    }
    HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), 1000);
    if(RobotState_Get()->operationMode == OP_MODE_AT)
        HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), 1000);
}

void handleSetSpeedCommand(const char *command) {
    char data[100];
    char wheel;
    int speed;
    if (sscanf(command + 12, "%c,%d", &wheel, &speed) == 2) {
        RobotState_SetWheelSpeed(wheel, speed);
        sprintf(data, "OK: Speed for wheel %c set to %d\r\n", wheel, speed);
    } else {
        sprintf(data, "ERROR: Invalid speed command format\r\n");
    }
    HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), 1000);
    if(RobotState_Get()->operationMode == OP_MODE_AT)
        HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), 1000);
}

void handleGetWheelCommand(const char *command) {
    char data[200];
    char wheel;
    if (sscanf(command + 12, "%c", &wheel) == 1) {
        WheelParams *params = (wheel == 'L') ? &RobotState_Get()->leftWheel : &RobotState_Get()->rightWheel;
        sprintf(data, "Wheel %c: Diameter=%.2f, Pulses=%d, MaxSpeed=%.2f, MinSpeed=%.2f, ControlMode=%d\r\n",
                wheel, params->diameter, params->pulses, params->maxSpeed, params->minSpeed, params->controlMode);
    } else {
        sprintf(data, "ERROR: Invalid wheel identifier\r\n");
    }
    HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), 1000);
    if(RobotState_Get()->operationMode == OP_MODE_AT)
        HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), 1000);
}

void handleGetOperationModeCommand(const char *command) {
    char data[100];
    OperationMode mode = RobotState_Get()->operationMode;
    const char* modeStr = (mode == OP_MODE_UART) ? "UART" :
                          (mode == OP_MODE_CAN) ? "CAN" :
                          (mode == OP_MODE_MICROROS) ? "MicroROS" : "AT";
    sprintf(data, "Current operation mode: %s\r\n", modeStr);
    HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), 1000);
    if(RobotState_Get()->operationMode == OP_MODE_AT)
        HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), 1000);
}

void handleSetWheelModeCommand(const char *command) {
    char data[100];
    char wheel;
    char modeStr[20];

    if (sscanf(command + 14, "%c,%s", &wheel, modeStr) == 2) {
        MotorControlMode mode;

        if (strcmp(modeStr, "OPEN") == 0) {
            mode = MOTOR_OPEN_LOOP;
        } else if (strcmp(modeStr, "CLOSE") == 0) {
            mode = MOTOR_CLOSE_LOOP;
        } else if (strcmp(modeStr, "ERROR") == 0) {
            mode = MOTOR_ERROR;
        } else if (strcmp(modeStr, "TEST") == 0) {
            mode = MOTOR_TEST;
        } else {
            sprintf(data, "ERROR: Invalid mode '%s'\r\n", modeStr);
            HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), 1000);
            if(RobotState_Get()->operationMode == OP_MODE_AT)
                HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), 1000);
            return;
        }

        if (wheel == 'L' || wheel == 'R') {
            RobotState_SetWheelControlMode(wheel, mode);
            sprintf(data, "OK: Control mode for wheel %c set to %s\r\n", wheel, modeStr);
        } else {
            sprintf(data, "ERROR: Invalid wheel identifier\r\n");
        }
    } else {
        sprintf(data, "ERROR: Invalid command format\r\n");
    }

    HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), 1000);
    if(RobotState_Get()->operationMode == OP_MODE_AT)
        HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), 1000);
}

void handleResetCommand(const char *command) {
    RobotState_Init();
    char data[50];
    sprintf(data, "OK: System reset to default settings\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), 1000);
    if(RobotState_Get()->operationMode == OP_MODE_AT)
        HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), 1000);
}

void handleRebootCommand(const char *command) {
    char data[50];
    sprintf(data, "OK: System rebooting...\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), HAL_MAX_DELAY); 
    if(RobotState_Get()->operationMode == OP_MODE_AT)
        HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), 1000);
    HAL_Delay(100);
    NVIC_SystemReset(); 
}
