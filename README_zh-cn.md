## 硬件

* 微控制器：STM32F407VET6

## 時鐘配置

* 系統時鐘：168 MHz
* AHB 頻率：168 MHz
* APB1 頻率：42 MHz
* APB2 頻率：84 MHz

## 启用的外设

* DMA：4 通道（2 个 USART1，2 个 USART2）
* FREERTOS
* GPIO：PA1、PA2、PA3、PA9、PA10、PC13、PD5、PD12
* TIM4
* USART1（波特率：9600）  microros slow
* USART2（波特率：1500000） microros high

## 引脚功能

| 引脚 | 功能 |
|---|---|
| PC13 | GPIO 输出 |
| PA1 | GPIO 输出 |
| PD5 | GPIO 输出 |
| PA2 | USART2 TX |
| PA3 | USART2 RX |
| PA9 | USART1 TX |
| PA10 | USART1 RX |
| PD12 | TIM4 CH1（PWM） |

## 其他配置

* HSE：25 MHz
* LSE：32.768 kHz
* SysTick：TIM1
* 默认任务优先级：24
* 默认任务堆栈大小：3000 字节


