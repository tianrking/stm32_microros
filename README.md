## Hardware

* Microcontroller: STM32F407VET6

## Clock Configuration

* System Clock: 168 MHz
* AHB Frequency: 168 MHz
* APB1 Frequency: 42 MHz
* APB2 Frequency: 84 MHz

## Enabled Peripherals

* ADC1
* CAN1
* DMA: 4 channels (2 for USART1, 2 for USART2)
* FREERTOS
* GPIO: Multiple pins configured
* I2C1, I2C2
* NVIC
* RCC
* SPI1, SPI2
* TIM3, TIM4, TIM9
* UART4, UART5
* USART1 (Baud Rate: 9600) microros slow
* USART2 (Baud Rate: 1500000) microros high
* USART3

## Pin Functions

| Pin | Function |
|-----|----------|
| PA0-WKUP | UART4 TX |
| PA1 | GPIO Output |
| PA2 | USART2 TX |
| PA3 | USART2 RX |
| PA5 | SPI1 SCK |
| PA6 | SPI1 MISO |
| PA7 | SPI1 MOSI |
| PA9 | USART1 TX |
| PA10 | USART1 RX |
| PA11 | CAN1 RX |
| PA12 | CAN1 TX |
| PC0 | ADCx_IN10 |
| PC1 | ADCx_IN11 |
| PC2-PC5 | ADCx_IN12 - ADCx_IN15 |
| PC6-PC7 | TIM3 CH1-CH2 |
| PC11 | UART4 RX |
| PC12 | UART5 TX |
| PC13 | GPIO Output |
| PD2 | UART5 RX |
| PD5 | GPIO Output |
| PD8 | USART3 TX |
| PD9 | USART3 RX |
| PD12 | TIM4 CH1 (PWM) |

## Additional Configuration

* HSE: 25 MHz
* LSE: 32.768 kHz
* SysTick: TIM1
* Default Task Priority: 24
* Default Task Stack Size: 3000 Bytes
