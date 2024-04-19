
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