// #ifndef __SPI_240_240_H__
// #define __SPI_240_240_H__

// #ifdef __cplusplus
// extern "C" {
// #endif


// #include "include.h" 


// //

// #define USE_HORIZONTAL 1
// // 修改这些宏定义以匹配你的硬件连接
// #define LCD_RES_GPIO_Port GPIOB
// #define LCD_RES_Pin GPIO_PIN_0
// #define LCD_DC_GPIO_Port  GPIOB
// #define LCD_DC_Pin  GPIO_PIN_2
// #define LCD_CS_GPIO_Port  GPIOB
// #define LCD_CS_Pin  GPIO_PIN_1

// // 定义控制宏
// #define LCD_RES_Clr() HAL_GPIO_WritePin(LCD_RES_GPIO_Port, LCD_RES_Pin, GPIO_PIN_RESET)
// #define LCD_RES_Set() HAL_GPIO_WritePin(LCD_RES_GPIO_Port, LCD_RES_Pin, GPIO_PIN_SET)

// #define LCD_DC_Clr()  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET)
// #define LCD_DC_Set()  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET)

// #define LCD_CS_Clr()  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET)
// #define LCD_CS_Set()  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)


// void LCD_Writ_Bus(uint8_t data);
// void LCD_WR_REG(uint8_t reg);
// void LCD_WR_DATA8(uint8_t data);
// void LCD_WR_DATA(uint16_t data);
// void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
// void LCD_Init(void);

// #endif