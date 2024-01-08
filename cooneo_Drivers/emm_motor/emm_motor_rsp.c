#include "include.h"

// 定义命令类型枚举
typedef enum {
    READ_MOTOR_REAL_TIME_POSITION = 0x36,
    READ_ENCODER_VALUE = 0x30,
    READ_INPUT_PULSES = 0x33,
    READ_POSITION_ERROR = 0x39,
    READ_ENABLE_STATUS = 0x3A,
    READ_STALL_FLAG = 0x3E
    // 可以在这里添加其他命令类型
} CommandType;

// 定义解析函数指针类型
typedef void (*parseFunction)(uint8_t*);

// 定义一个结构体来关联命令类型和解析函数
typedef struct {
    CommandType cmdType;
    parseFunction parser;
} CommandParser;

// 创建一个全局的命令解析表
CommandParser commandParsers[] = {
    {READ_MOTOR_REAL_TIME_POSITION, parseMotorRealTimePosition},
    {READ_ENCODER_VALUE, parseEncoderValue},
    {READ_INPUT_PULSES, parseInputPulses},
    {READ_POSITION_ERROR, parsePositionError},
    {READ_ENABLE_STATUS, parseEnableStatus},
    {READ_STALL_FLAG, parseStallFlag},
    // 在这里可以添加其他命令和解析函数
};

int e1,e2,e3,e4,e5,e6,ee;
// 实现电机实时位置解析函数
void parseMotorRealTimePosition(uint8_t* data) {
    // 解析数据，转换成 int32_t 类型
    int32_t motorPosition = ((int32_t)data[1] << 24) | 
                            ((int32_t)data[2] << 16) |
                            ((int32_t)data[3] << 8) |
                            ((int32_t)data[4]);
    // 进行其他必要的处理，例如转换成角度等
    double angle = (double)motorPosition * 360 / 65536;
    (void)angle;
    e1++;
    // TODO: 在这里添加你的处理逻辑，例如更新UI或变量等
}

void parseEncoderValue(uint8_t* data) {
    // Assuming data[1] and data[2] are the bytes representing the encoder value
    uint16_t encoderValue = ((uint16_t)data[1] << 8) | (uint16_t)data[2];
    (void)encoderValue;
    // TODO: Add the logic to handle or display the encoder value
    e2++;
}

void parseInputPulses(uint8_t* data) {
    // Assuming data[1] to data[4] are the bytes representing the input pulse count
    int32_t inputPulses = ((int32_t)data[1] << 24) |
                          ((int32_t)data[2] << 16) |
                          ((int32_t)data[3] << 8)  |
                          (int32_t)data[4];

    (void)inputPulses;
    e3++;
    // TODO: Add the logic to handle or display the input pulses
}

void parsePositionError(uint8_t* data) {
    int16_t positionError = ((int16_t)data[1] << 8) | (int16_t)data[2];
    // Convert the position error to angle if necessary
    double angleError = (double)positionError * 360 / 65536;
    (void)angleError;
    e4++;
    // TODO: Add the logic to handle or display the position error
}

void parseEnableStatus(uint8_t* data) {
    uint8_t enableStatus = data[1]; // Assuming data[1] is the enable status byte
    (void)enableStatus;
    // TODO: Add the logic to handle or display the enable status
    e5++;
}

void parseStallFlag(uint8_t* data) {
    uint8_t stallFlag = data[1]; // Assuming data[1] is the stall flag byte
    (void)stallFlag;
    // TODO: Add the logic to handle or display the stall flag
    e6++;
}

// 实现错误处理函数
void handleError(int errorCode) {
    switch (errorCode) {
        case ERR_INVALID_DATA:
            // 处理无效数据错误
            break;
        case ERR_UNKNOWN_CMD:
            // 处理未知命令错误
            break;
        case ERR_BUFFER_OVERFLOW:
            // 处理缓冲区溢出错误
            break;
        // 添加其他错误处理逻辑
        default:
            // 处理未知错误
            break;
    }
    // 可以添加错误日志记录或通知用户代码
}

// 分发函数实现
void dispatchParser(uint8_t* receivedData, size_t dataSize) {
    if(dataSize < 1) {
        handleError(ERR_INVALID_DATA);
        return;
    }

    uint8_t cmdType = receivedData[0];
    for (int i = 0; i < sizeof(commandParsers) / sizeof(CommandParser); ++i) {
        if (commandParsers[i].cmdType == cmdType) {
            commandParsers[i].parser(receivedData);  //not arrive
            return;
        }
    }
    ee++; //arrived
    handleError(ERR_UNKNOWN_CMD);
}

uint8_t dataBuffer;
int eee,eee1;

// UART接收中断回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    static uint8_t receivedData[MAX_DATA_LENGTH];
    static size_t currentLength = 0;
    eee ++;
    if (huart->Instance == USART1) {  // 确保是UART1的中断
        eee1++;
        if (currentLength < MAX_DATA_LENGTH) {
            receivedData[currentLength++] = dataBuffer;

            if(dataBuffer == 0x6B || currentLength == MAX_DATA_LENGTH) { // 假设0x6B是结束标志或缓冲区已满
                dispatchParser(receivedData, currentLength);
                currentLength = 0; // 重置长度，准备下一次接收
            }
        } else {
            currentLength = 0; // 重置长度
        }

        // 继续接收下一个字节
        HAL_UART_Receive_IT(huart, &dataBuffer, 1);
    }
}

//  Recieve is Wong