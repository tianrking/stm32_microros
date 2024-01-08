#include "include.h"


// 触发部分
void emm_TriggerEncoderCalibration(uint8_t motorId) {
    uint8_t command[] = {motorId, 0x06, 0x45, 0x6B}; // 命令数据
    HAL_UART_Transmit(&huart1, command, sizeof(command), 10); // 发送命令
}

void emm_SetCurrentPositionToZero(uint8_t motorId) {
    uint8_t command[] = {motorId, 0x0A, 0x6D, 0x6B}; // 命令数据
    HAL_UART_Transmit(&huart1, command, sizeof(command), 10); // 发送命令
}

void emmReleaseStallProtection(uint8_t motorId) {
    uint8_t command[] = {motorId, 0x0E, 0x52, 0x6B}; // 命令数据
    HAL_UART_Transmit(&huart1, command, sizeof(command), 10); // 发送命令
}

// 读取参数部分
int  f1,f2,f3,f4,f5,f6,f7;
void emm_ReadEncoderValue(uint8_t motorId) {
    uint8_t command[] = {motorId, 0x30, 0x6B}; // 命令数据
    HAL_UART_Transmit(&huart1, command, sizeof(command), 10); // 发送命令
    f1++;
    // 接下来，你需要实现接收逻辑并处理返回的数据
}

void emm_ReadInputPulses(uint8_t motorId) {
    uint8_t command[] = {motorId, 0x33, 0x6B}; // 命令数据
    HAL_UART_Transmit(&huart1, command, sizeof(command), 10); // 发送命令
    f2++;
    // 接下来，你需要实现接收逻辑并处理返回的数据
}

void emm_ReadMotorRealTimePosition(uint8_t motorId) {
    uint8_t command[] = {motorId, 0x36, 0x6B}; // 命令数据
    HAL_UART_Transmit(&huart1, command, sizeof(command), 10); // 发送命令
    f3++;
    // 接下来，你需要实现接收逻辑并处理返回的数据
}

void emm_ReadPositionError(uint8_t motorId) {
    uint8_t command[] = {motorId, 0x39, 0x6B}; // 命令数据
    HAL_UART_Transmit(&huart1, command, sizeof(command), 10); // 发送命令
    // 接下来，你需要实现接收逻辑并处理返回的数据
}

void emm_ReadEnableStatus(uint8_t motorId) {
    uint8_t command[] = {motorId, 0x3A, 0x6B}; // 命令数据
    HAL_UART_Transmit(&huart1, command, sizeof(command), 10); // 发送命令
    // 接下来，你需要实现接收逻辑并处理返回的数据
}

void emm_ReadStallFlag(uint8_t motorId) {
    uint8_t command[] = {motorId, 0x3E, 0x6B}; // 命令数据
    HAL_UART_Transmit(&huart1, command, sizeof(command), 10); // 发送命令
    // 接下来，你需要实现接收逻辑并处理返回的数据
}

void emm_ReadAutoHomeStatus(uint8_t motorId) {
    uint8_t command[] = {motorId, 0x3F, 0x6B}; // 命令数据
    HAL_UART_Transmit(&huart1, command, sizeof(command), 10); // 发送命令
    // 接下来，你需要实现接收逻辑并处理返回的数据
}


// 控制部分

void emm_ControlMotorEnable(uint8_t motorId,uint8_t enableStatus) {
    uint8_t command[] = {motorId, 0xF3, enableStatus, 0x6B}; // 命令数据
    HAL_UART_Transmit(&huart1, command, sizeof(command), 10); // 发送命令
}

void emm_ControlMotorDirectionSpeed(uint8_t motorId,uint16_t directionSpeed, uint8_t acceleration) {
    uint8_t command[] = {motorId, 0xF6, directionSpeed >> 8, directionSpeed & 0xFF, acceleration, 0x6B}; // 命令数据
    HAL_UART_Transmit(&huart1, command, sizeof(command), 10); // 发送命令
}

void emm_StoreOrClearParameters(uint8_t motorId,uint8_t operation) {
    uint8_t command[] = {motorId, 0xFF, operation, 0x6B}; // 命令数据
    HAL_UART_Transmit(&huart1, command, sizeof(command), 10); // 发送命令
}

void emm_ControlMotorRelativeAngle(uint8_t motorId,uint16_t directionSpeed, uint8_t acceleration, uint32_t pulseCount) {
    // uint8_t command[] = {
    //     motorId, 0xFD,
    //     directionSpeed >> 8, directionSpeed & 0xFF,
    //     acceleration,
    //     pulseCount >> 16, (pulseCount >> 8) & 0xFF, pulseCount & 0xFF,
    //     0x6B
    // }; // 命令数据
    uint8_t command[] = {
        motorId, 0xFD,
        directionSpeed >> 8, directionSpeed & 0xFF,
        acceleration,
        (uint8_t)(pulseCount >> 16),       // 最高位字节
        (uint8_t)((pulseCount >> 8) & 0xFF), // 中间字节
        (uint8_t)(pulseCount & 0xFF),       // 最低位字节
        0x6B
    }; 

    //01 FD 14 FF 00 00 0C 80 6B

    // uint8_t command[] = {
    //     0x02, 0xFD,
    //     0x14, 0xFF,
    //     0x00,
    //     0x00, 0x06, 0x40,
    //     0x6B
    // }; // 命令数据

    HAL_UART_Transmit(&huart1, command, sizeof(command), 10); // 发送命令
}

// 计算所需脉冲数的函数
// uint32_t calculatePulseCount(double desiredAngle, double anglePerPulse) {
//     return (uint32_t)(desiredAngle / anglePerPulse);
// }
uint32_t calculatePulseCount(double desiredAngle, double anglePerPulse) {
    // 使用四舍五入以避免因浮点数除法带来的误差
    return (uint32_t)(desiredAngle / anglePerPulse + 0.5);
}

// 上层接口
void emm_ControlMotorToAngle(uint8_t motorId, double desiredAngle) {
    
    //double desiredAngle = 90.0; // 目标旋转角度
    //desiredAngle = 180;
    double anglePerPulse = 1.8 / 16; // 每个脉冲的旋转角度
    uint16_t directionSpeed = 0x0100; // 设置一个方向和速度值
    uint8_t acceleration = 0x01; // 设置加速度值sss

    // 计算脉冲数
    uint32_t pulseCount = calculatePulseCount(desiredAngle, anglePerPulse);
    emm_ControlMotorRelativeAngle(motorId, directionSpeed, acceleration, pulseCount);
}

// tools

uint16_t convertToDirectionSpeed(int speed) {
    uint16_t directionSpeed;
    uint16_t absSpeed = (speed >= 0) ? speed : -speed; // 获取速度的绝对值

    // 限制速度值范围（例如，电机的实际速度范围可能是 0x0000 - 0x04FF）
    if (absSpeed > 0x04FF) {
        absSpeed = 0x04FF;
    }

    // 设置方向位（最高半字节）
    // 假设：正数为正转（0x0），负数为反转（0x1）
    directionSpeed = (speed >= 0) ? 0x0000 : 0x1000;

    // 结合方向和速度
    directionSpeed |= absSpeed;

    return directionSpeed;
}

