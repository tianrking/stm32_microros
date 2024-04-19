#include "AT.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include "robot_state.h"

// ������������
void handleOperationModeCommand(const char *command);
void handleWheelCommand(const char *command);
void handleSetSpeedCommand(const char *command);
void handleGetWheelCommand(const char *command);
void handleGetOperationModeCommand(const char *command);
void handleSetWheelModeCommand(const char *command);
void handleResetCommand(const char *command);
void handleRebootCommand(const char *command); //system level
// ���ұ����
typedef struct {
    const char *commandPrefix;
    void (*handler)(const char*);
} CommandHandler;

// ���ұ�
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
    char data[50];
    if (strcmp(command + 10, "UART") == 0) {
        RobotState_SetOperationMode(OP_MODE_UART);
        sprintf(data, "OK: UART mode set\r\n");
    } else if (strcmp(command + 10, "CAN") == 0) {
        RobotState_SetOperationMode(OP_MODE_CAN);
        sprintf(data, "OK: CAN mode set\r\n");
    } else if (strcmp(command + 10, "MICROROS") == 0) {
        RobotState_SetOperationMode(OP_MODE_MICROROS);
        sprintf(data, "OK: MicroROS mode set\r\n");
    } else if (strcmp(command + 10, "AT") == 0) {
        RobotState_SetOperationMode(OP_MODE_AT);
        sprintf(data, "OK: AT command mode set\r\n");
    } else {
        sprintf(data, "ERROR: Invalid operation mode\r\n");
    }
    HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), 1000);
    if(RobotState_Get()->operationMode == OP_MODE_AT)
        HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), 1000);
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

    // ���Խ�������
    if (sscanf(command + 14, "%c,%s", &wheel, modeStr) == 2) {
        MotorControlMode mode;

        // ת���ַ�����ö��
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

        // ����ģʽ
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
    HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), HAL_MAX_DELAY); // ȷ����Ϣ�������
    if(RobotState_Get()->operationMode == OP_MODE_AT)
        HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), 1000);
    HAL_Delay(100);  // ����ʱ�䷢�����
    NVIC_SystemReset();  // ����ϵͳ����
}
