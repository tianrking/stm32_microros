
# STM32F407 Debugging with GDB

## Prerequisites

### Hardware
- STM32F407VET6 Development Board
- ST-Link/V2 Debugger

### Software
- [GCC ARM Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
- [Download ARM GNU Toolchain](https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz)
- OpenOCD
- gdb-multiarch

## Debugging Steps

### 1. Connect Hardware
Connect your STM32F407VET6 board and ST-Link/V2 to your PC.

### 2. Start OpenOCD
```bash
openocd -f interface/stlink-v2.cfg -f target/stm32f4x.cfg
```

### 3. Start gdb-multiarch
```bash
gdb-multiarch
target remote localhost:3333
target extended-remote :3333
```

### 4. Load and Debug Your Program
```gdb
file /path/to/your/firmware.elf
load
break main
continue
```

## Commands

### Inspecting and Modifying Variables
```gdb
print variableName
set variable variableName = newValue
watch variableName
```

### Managing Breakpoints
```gdb
break [filename]:[linenumber]
delete [breakpoint number]
disable [breakpoint number]
enable [breakpoint number]
info breakpoints
break [location] if [condition]
```

### Debugging Control Commands
```gdb
monitor reset halt  # Hardware Reset
monitor reset        # Software Reset
monitor halt         # Halting Execution
continue             # Continuing Execution
```

## Additional Tips
- Check all connections and configurations before starting.
- Ensure your firmware is correctly compiled and formatted.
- Understand basic GDB commands for an effective debugging session.
- Keep your toolchain and utilities updated.

By following these steps, you will be able to effectively debug your STM32F407 firmware using gdb-multiarch on a Linux system.


## Micros Integration

```bash
docker run -it --rm -v /dev:/dev --privileged --net=host microros/micro-ros-agent:iron serial --dev /dev/ttyUSB0 -b 115200
```

## Bug

```
w0x7ce@w0x7ce-ASUS-TUF-Gaming-F15-FX506HC-FX506HC:~/STM32CubeMX$ docker run -it --rm -v /dev:/dev --privileged --net=host microros/micro-ros-agent:humble serial --dev /dev/ttyUSB0 -b 115200
[1730772857.947723] info     | TermiosAgentLinux.cpp | init                     | running...             | fd: 3
[1730772857.947970] info     | Root.cpp           | set_verbose_level        | logger setup           | verbose_level: 4
[1730772860.549717] info     | Root.cpp           | create_client            | create                 | client_key: 0x5851F42D, session_id: 0x81
[1730772860.549801] info     | SessionManager.hpp | establish_session        | session established    | client_key: 0x5851F42D, address: 0
[1730772860.567938] info     | ProxyClient.cpp    | create_participant       | participant created    | client_key: 0x5851F42D, participant_id: 0x000(1)
[1730772860.584608] info     | ProxyClient.cpp    | create_topic             | topic created          | client_key: 0x5851F42D, topic_id: 0x000(2), participant_id: 0x000(1)
[1730772860.594713] info     | ProxyClient.cpp    | create_publisher         | publisher created      | client_key: 0x5851F42D, publisher_id: 0x000(3), participant_id: 0x000(1)
[1730772860.607154] info     | ProxyClient.cpp    | create_datawriter        | datawriter created     | client_key: 0x5851F42D, datawriter_id: 0x000(5), publisher_id: 0x000(3)
terminate called after throwing an instance of 'eprosima::fastcdr::exception::NotEnoughMemoryException'
  what():  Not enough memory in the buffer stream
[ros2run]: Aborted
```



## ROS2 话题列表

### 订阅话题（可发送指令的话题）
1. 单轮速度控制
```bash
# 控制右轮速度（单位：m/s）
ros2 topic pub wheel_right/target std_msgs/msg/Float64 "data: 0.1"

# 控制左轮速度（单位：m/s）
ros2 topic pub wheel_left/target std_msgs/msg/Float64 "data: 0.15"
```

2. 运动控制（质点模型）
```bash
# 话题：/cmd_vel
# 消息类型：geometry_msgs/msg/Twist
# 说明：控制机器人的线速度和角速度

# 示例1：前进（线速度为0.2米/秒）
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist "{linear: {x: 0.2, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.0}}"

# 示例2：原地旋转（角速度为0.5弧度/秒，大约28.6度/秒）
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist "{linear: {x: 0.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.5}}"

# 示例3：前进并转弯（线速度0.2米/秒，角速度0.3弧度/秒）
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist "{linear: {x: 0.2, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.3}}"
```

3. PID参数设置
```bash
# 话题：/pid_params
# 消息类型：std_msgs/msg/String
# 说明：通过JSON字符串设置电机PID参数

# 示例1：设置基础PID参数
ros2 topic pub /pid_params std_msgs/msg/String "data: '{\"p\":1.0,\"i\":0.5,\"d\":0.1}'"

# 示例2：设置更激进的PID参数
ros2 topic pub /pid_params std_msgs/msg/String "data: '{\"p\":3.0,\"i\":2.0,\"d\":0.2}'"
```

4. 车辆参数配置
```bash
# 话题：/vehicle_params
# 消息类型：std_msgs/msg/String
# 格式：'<驱动类型>,<车轮半径>,<轮距>,<轴距>'
# 单位：所有距离参数均为米

# 差速驱动配置（默认）
ros2 topic pub /vehicle_params std_msgs/msg/String "data: 'differential,0.065,0.32,0.32'"

# 麦克纳姆轮配置
ros2 topic pub /vehicle_params std_msgs/msg/String "data: 'mecanum,0.065,0.32,0.32'"

# 阿克曼转向配置
ros2 topic pub /vehicle_params std_msgs/msg/String "data: 'ackermann,0.065,0.32,0.32'"
```

### 发布话题（可订阅的反馈信息）
1. 轮速反馈
```bash
# 右轮实际速度（m/s）
ros2 topic echo wheel_right/feedback

# 右轮目标速度（m/s）
ros2 topic echo wheel_right/target_speed

# 左轮实际速度（m/s）
ros2 topic echo wheel_left/feedback

# 左轮目标速度（m/s）
ros2 topic echo wheel_left/target_speed
```

2. 调试信息
```bash
# 系统心跳消息
ros2 topic echo ping_ping
```

## 参数限制与说明

### 速度限制
- 最大线速度（MAX_LINEAR_SPEED）：±5.0 m/s
- 最大角速度（MAX_ANGULAR_SPEED）：±180.0 度/秒

### 机器人物理参数
- 默认轮距（WHEEL_SEPARATION）：0.2 m
- 默认轮半径（WHEEL_RADIUS）：0.02 m

### 驱动类型
- differential：差速驱动，适用于两轮差速机器人
- mecanum：麦克纳姆轮，适用于全向移动机器人
- ackermann：阿克曼转向，适用于汽车类转向机构

## 反馈数据说明
1. wheel_*/feedback：实际轮子速度，单位 m/s
   - 正值表示前进
   - 负值表示后退
   
2. wheel_*/target_speed：目标轮子速度，单位 m/s
   - 显示当前设定的目标速度
   - 用于验证控制指令是否正确接收

## 常见使用场景
1. 基本运动控制
```bash
# 直线前进（0.2m/s）
ros2 topic pub -1 /cmd_vel geometry_msgs/msg/Twist "{linear: {x: 0.2, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.0}}"

# 原地左转（45度/秒 ≈ 0.785弧度/秒）
ros2 topic pub -1 /cmd_vel geometry_msgs/msg/Twist "{linear: {x: 0.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.785}}"

# 停止运动
ros2 topic pub -1 /cmd_vel geometry_msgs/msg/Twist "{linear: {x: 0.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.0}}"
```

2. 调试场景
```bash
# 监测实际速度与目标速度的差异
ros2 topic echo wheel_left/feedback &
ros2 topic echo wheel_left/target_speed

# PID参数微调示例
ros2 topic pub /pid_params std_msgs/msg/String "data: '{\"p\":1.5,\"i\":0.8,\"d\":0.15}'"
```

## 注意事项
1. 系统默认使用差速驱动（differential）模式
2. 所有尺寸参数（车轮半径、轮距、轴距）均使用米为单位
3. PID参数可以实时调整，建议从小参数开始调试
4. 速度值的单位统一使用米/秒（m/s）和弧度/秒（rad/s）
5. 更改车身参数或驱动模式后，系统会自动使用相应的运动学模型进行速度解算
6. 实际速度会受到硬件限制，请根据实际情况设置合适的速度值
7. 在进行PID调试时，建议先使用较小的速度值进行测试
8. 修改车辆参数后，建议先进行低速测试以验证配置是否正确