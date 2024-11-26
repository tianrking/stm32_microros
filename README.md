###  查看 DEBUG.md

查看 DEBUG.md 详细讲解 microros使用此套件 AT命令 目前没有使用， 是老版本控制使用的

### AT命令测试集合及期望响应

#### 设置操作模式
- **命令:** `AT+OPMODE=MODE`
  - **参数说明:**
    - `UART` - 串口控制模式
    - `CAN` - CAN总线模式
    - `MICROROS` - MicroROS模式
    - `AT` - AT指令模式
  - **发送和预期响应:**
    - `AT+OPMODE=UART`
      - **期望正常响应:** `OK: UART mode set`
      - **错误命令:** `AT+OPMODE=UARTX`
      - **期望错误响应:** `ERROR: Invalid operation mode`
    - `AT+OPMODE=CAN`
      - **期望正常响应:** `OK: CAN mode set`
      - **错误命令:** `AT+OPMODE=CANY`
      - **期望错误响应:** `ERROR: Invalid operation mode`
    - `AT+OPMODE=MICROROS`
      - **期望正常响应:** `OK: MicroROS mode set`
      - **错误命令:** `AT+OPMODE=MICROROSX`
      - **期望错误响应:** `ERROR: Invalid operation mode`
    - `AT+OPMODE=AT`
      - **期望正常响应:** `OK: AT command mode set`
      - **错误命令:** `AT+OPMODE=ATX`
      - **期望错误响应:** `ERROR: Invalid operation mode`

#### 设置轮子参数
- **命令:** `AT+SETWHEEL=WHEEL,Diameter,Pulses,MaxSpeed,MinSpeed,Mode`
  - **参数说明:**
    - `WHEEL` - 轮子标识 (`L` 左轮, `R` 右轮)
    - `Diameter` - 轮径
    - `Pulses` - 编码器脉冲数
    - `MaxSpeed` - 最大速度
    - `MinSpeed` - 最小速度
    - `Mode` - 控制模式 (0 = 开环, 1 = 闭环, 2 = 错误, 3 = 测试)
  - **发送和预期响应:**
    - `AT+SETWHEEL=L,0.5,100,3.5,0.5,1`
      - **期望正常响应:** `OK: Left wheel set`
      - **错误命令:** `AT+SETWHEEL=L,0.5,100,3.5`
      - **期望错误响应:** `ERROR: Invalid wheel parameters`
    - `AT+SETWHEEL=R,0.5,100,3.5,0.5,1`
      - **期望正常响应:** `OK: Right wheel set`
      - **错误命令:** `AT+SETWHEEL=R,X,100,3.5,0.5,1`
      - **期望错误响应:** `ERROR: Invalid wheel parameters`

#### 设置轮子速度
- **命令:** `AT+SETSPEED=WHEEL,Speed`
  - **参数说明:**
    - `WHEEL` - 轮子标识 (`L` 左轮, `R` 右轮)
    - `Speed` - 速度值
  - **发送和预期响应:**
    - `AT+SETSPEED=L,100`
      - **期望正常响应:** `OK: Speed for wheel L set to 100`
      - **错误命令:** `AT+SETSPEED=L,X`
      - **期望错误响应:** `ERROR: Invalid speed command format`
    - `AT+SETSPEED=R,100`
      - **期望正常响应:** `OK: Speed for wheel R set to 100`
      - **错误命令:** `AT+SETSPEED=R,`
      - **期望错误响应:** `ERROR: Invalid speed command format`

#### 查询轮子参数
- **命令:** `AT+GETWHEEL=WHEEL`
  - **参数说明:**
    - `WHEEL` - 轮子标识 (`L` 左轮, `R` 右轮)
  - **发送和预期响应:**
    - `AT+GETWHEEL=L`
      - **期望正常响应:** `Wheel L: Diameter=0.5, Pulses=100, MaxSpeed=3.5, MinSpeed=0.5, ControlMode=1`
      - **错误命令:** `AT+GETWHEEL=X`
      - **期望错误响应:** `ERROR: Invalid wheel identifier`
    - `AT+GETWHEEL=R`
      - **期望正常响应:** `Wheel R: Diameter=0.5, Pulses=100, MaxSpeed=3.5, MinSpeed=0.5, ControlMode=1`
      - **错误命令:** `AT+GETWHEEL=Y`
      - **期望错误响应:** `ERROR: Invalid wheel identifier`

#### 查询当前操作模式
- **命令:** `AT+GETOPMODE`
  - **发送:** `AT+GETOPMODE`
  - **期望正常响应:** `Current operation mode: AT` (或其他根据实际设置的模式变化)
  - **错误命令:** `AT+GETOPMODE=XYZ`
  - **期望错误响应:** `ERROR: Unknown command`

#### 重置系统配置
- **命令:** `AT+RESET`
  - **发送:** `AT+RESET`
  - **期望正常响应:** `OK: System reset to default settings`
  - **错误命令:** `AT+RESETX`
  - **期望错误响应:** `ERROR: Unknown command`

