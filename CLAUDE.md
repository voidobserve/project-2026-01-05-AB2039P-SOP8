# CLAUDE.md

本文件为 Claude Code (claude.ai/code) 在处理本仓库代码时提供指导。

## 项目概述

这是一个基于 **AB2039P 微控制器**（RISC-V 32位）的嵌入式固件项目，专注于车控照明，具备低功耗蓝牙（BLE）功能。

**硬件平台：** AB2039P RISC-V 32位 MCU（`rv32imacxbs1` 架构）
**主要应用：** 车载照明控制的 BLE 从设备
**开发语言：** C（代码注释为中文）

## 编译命令

主项目位于 `project/ble_peripheral/`。使用 Make 编译（Windows 上需要 MSYS2/MinGW）：

```bash
# 首先进入项目目录
cd project/ble_peripheral

# 完整编译（包含编译前后批处理脚本）
make all

# 快速重新编译（跳过批处理脚本）
make build

# 清理编译产物
make clean

# 完整重新编译
make rebuild

# 显示帮助
make help
```

**工具链：** RISC-V 32位 GCC 位于 `C:\PROGRA~2\RV32-T~1\RV32-V1\bin\riscv32-elf-*`

**输出文件：**
- `Output/bin/app.rv32` - ELF 可执行文件
- `Output/bin/app.bin` - 原始二进制文件
- `Output/bin/app.dcf` - 最终固件镜像
- `Output/bin/map.txt` - 链接器映射文件

**IDE：** 每个项目/示例都提供了 Code::Blocks 工程文件（`.cbp`）。

## 架构

### 目录结构

```
app/
├── driver/          # 硬件驱动（GPIO、PWM、UART、SPI、ADC 等）
├── header/          # 系统头文件和 API 定义
├── libs/            # 静态库
│   ├── ble/         # BLE 协议栈（libbtstack.a）
│   ├── cpu/         # 平台库（libc、libm、libgcc、libplatform）
│   └── wireless/    # 无线通信库
├── modules/         # 专用模块（IIR 滤波器、Flash 追踪）
├── profiles/        # BLE 服务配置文件（25+ 标准 GATT 服务）
└── project/
    ├── ble_peripheral/   # 主 BLE 从设备项目
    ├── ble_central/      # BLE 主设备项目
    ├── example/          # 硬件示例（PWM、LED、UART 等）
    ├── solution/         # 完整解决方案（HID、遥控器、适配器）
    └── wireless_*/       # 无线项目
```

### ble_peripheral 项目结构

```
project/ble_peripheral/
├── app/         # 应用逻辑（func_*.c - 模式处理函数）
├── ble/         # BLE 协议栈集成（广播、服务、配置文件）
├── bsp/         # 板级支持（按键、ADC、UART、PWM 封装）
├── prod_test/   # 产测模块（IODM、TBOX）
├── config.h     # 主配置文件
├── main.c       # 程序入口
└── ram.ld       # 链接脚本
```

### 代码流程

1. `main.c` → `sys_rst_init()` → `bsp_sys_init()` → `prod_test_init()` → `func_run()`
2. `func_run()` 通过 `func_cb.sta` 状态机管理运行模式
3. BLE 协议栈运行在专用操作系统线程（`OS_THREAD_BT_EN`）

### 关键配置

**`config.h`** 控制所有主要功能：
- `FUNC_BT_EN` - 启用蓝牙功能
- `FUNC_IODM_EN` / `FUNC_TBOX_EN` - 产测模式
- `SYS_CLK_SEL` - 系统时钟（默认：24MHz）
- `PMU_CFG_VBAT` - 电源配置
- `SYS_SLEEP_EN` - 低功耗模式
- `AB_FOT_EN` - FOTA（空中升级）支持
- `FLASH_SIZE` - Flash 存储器大小（256K/512K）

### 存储器布局

- Flash 基址：`0x10000000`
- 堆栈：`0x10800`（1KB）
- SRAM：`0x10c00`（21KB）
- Flash 代码最大值：通过 `FLASH_SIZE - FLASH_RESERVE_SIZE` 配置

## 硬件驱动

位于 `driver/` 目录，所有文件均以 `driver_` 为前缀：
- `driver_gpio.c/h` - GPIO 控制
- `driver_pwm.c/h` - PWM 输出（用于 LED 控制）
- `driver_uart.c/h` - UART 通信
- `driver_saradc.c/h` - ADC 采样
- `driver_lowpwr.c/h` - 低功耗管理
- `driver_hsuart.c/h` - 高速 UART
- `driver_spi.c/h` - SPI 接口
- `driver_tmr.c/h` - 硬件定时器
- `driver_rtc.c/h` - 实时时钟
- `driver_wdt.c/h` - 看门狗定时器

## GPIO 引脚命名

引脚定义在 `config_define.h` 中：
- `GPIO_NULL` (0x00) - 禁用
- `GPIO_PB0` 到 `GPIO_PB9` (0xB0-0xB9) - B 端口引脚

## BLE 配置文件

可用的配置文件位于 `profiles/` 目录 - 将需要的服务文件夹复制到项目的 `ble/` 目录中：
- 电池服务、HID 服务、心率、设备信息
- Apple ANCS/AMS 通知服务
- 更多标准 GATT 服务

## 产品测试

`prod_test/` 中有两个测试框架：
- **IODM** - 通过 VUSB 引脚的单线 UART 测试，波特率 115200
- **TBOX** - 用于制造的测试盒协议

通过 `config.h` 中的 `FUNC_IODM_EN` 或 `FUNC_TBOX_EN` 启用。

## FOTA（空中升级）

当 `AB_FOT_EN=1` 时：
- 256K flash：升级包位于 `0x23000`（100KB）
- 512K flash：升级包位于 `0x4B000`（200KB）
