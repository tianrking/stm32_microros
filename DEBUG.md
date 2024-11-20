
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

ros2 topic pub wheel_right/target_speed std_msgs/msg/Float64 "data: 500" # 设置电机1为50% PWM
ros2 topic pub wheel_left/target_speed std_msgs/msg/Float64 "data: 1000" # 设置电机2为100% PWM

ros2 topic pub pid_params std_msgs/msg/Float32MultiArray "data: [2.5, 3.2, 0.05]" 测试PID

ros2 topic pub /vehicle_params std_msgs/msg/String "data: 'mecanum,0.065,0.32,0.32'"
ros2 topic pub /vehicle_params std_msgs/msg/String "data: 'ackermann,0.065,0.32,0.32'"
ros2 topic pub /vehicle_params std_msgs/msg/String "data: 'differential,0.065,0.32,0.32'"




print state_manager