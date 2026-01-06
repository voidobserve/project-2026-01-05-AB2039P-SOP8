# ============================================================================
# pwm 示例项目 Makefile
# 从 Code::Blocks 配置转换生成
# 目标平台: RISC-V 32位 (AB2039P)
# ============================================================================

# 项目名称
PROJECT_NAME = pwm

# 工具链路径 (使用短路径避免空格和括号问题)
TOOLCHAIN_PATH = /c/PROGRA~2/RV32-T~1/RV32-V1/bin
CROSS_COMPILE = $(TOOLCHAIN_PATH)/riscv32-elf-
CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
XMAKER  = $(CROSS_COMPILE)xmaker

# 导出 PATH 环境变量供批处理文件使用
export PATH := $(TOOLCHAIN_PATH):$(PATH)

# 目录定义
TOP_DIR     = ../../..
DRIVER_DIR  = $(TOP_DIR)/driver
HEADER_DIR  = $(TOP_DIR)/header
LIBS_DIR    = $(TOP_DIR)/libs

OUT_DIR     = Output
BIN_DIR     = $(OUT_DIR)/bin
OBJ_DIR     = $(OUT_DIR)/obj

# 输出文件
TARGET      = $(BIN_DIR)/app.rv32
TARGET_BIN  = $(BIN_DIR)/app.bin
TARGET_DCF  = $(BIN_DIR)/app.dcf
MAP_FILE    = $(BIN_DIR)/map.txt
LINKER_SCR  = $(OBJ_DIR)/ram.o

# 编译器选项
CFLAGS = -Os -Wall
CFLAGS += -march=rv32imacxbs1
CFLAGS += -ffunction-sections
CFLAGS += -msave-restore
CFLAGS += -mjump-tables-in-text

# 头文件包含路径
INCLUDES = -I.
INCLUDES += -I$(HEADER_DIR)
INCLUDES += -I$(LIBS_DIR)
INCLUDES += -I$(LIBS_DIR)/cpu
INCLUDES += -I$(LIBS_DIR)/ble
INCLUDES += -I$(LIBS_DIR)/usb
INCLUDES += -I$(DRIVER_DIR)
INCLUDES += -Ihardware

# 链接器选项
LDFLAGS = -T$(LINKER_SCR)
LDFLAGS += --gc-sections
LDFLAGS += -Map=$(MAP_FILE)

# 静态库
LIBS = $(LIBS_DIR)/cpu/libplatform.a
LIBS += $(LIBS_DIR)/cpu/libm.a
LIBS += $(LIBS_DIR)/cpu/libc.a
LIBS += $(LIBS_DIR)/cpu/libgcc.a

# ============================================================================
# 源文件
# ============================================================================

# 驱动源文件
DRIVER_SRCS = \
	$(DRIVER_DIR)/driver_clk.c \
	$(DRIVER_DIR)/driver_gpio.c \
	$(DRIVER_DIR)/driver_pwm.c \
	$(DRIVER_DIR)/driver_uart.c

# 硬件抽象层源文件
HARDWARE_SRCS = \
	hardware/bsp_uart_debug.c \
	hardware/pwm.c

# 主程序源文件
MAIN_SRCS = \
	main.c

# 所有源文件
SRCS = $(DRIVER_SRCS) $(HARDWARE_SRCS) $(MAIN_SRCS)

# ============================================================================
# 目标文件
# ============================================================================

# 驱动目标文件
DRIVER_OBJS = $(patsubst $(DRIVER_DIR)/%.c,$(OBJ_DIR)/driver/%.o,$(DRIVER_SRCS))

# 硬件抽象层目标文件
HARDWARE_OBJS = $(patsubst hardware/%.c,$(OBJ_DIR)/hardware/%.o,$(HARDWARE_SRCS))

# 主程序目标文件
MAIN_OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(MAIN_SRCS))

# 所有目标文件
OBJS = $(DRIVER_OBJS) $(HARDWARE_OBJS) $(MAIN_OBJS)

# ============================================================================
# 构建规则
# ============================================================================

.PHONY: all clean postbuild directories build

# 默认目标 - 完整构建 (包含后处理步骤)
all: build postbuild
	@echo "============================================"
	@echo "构建完成: $(TARGET)"
	@echo "============================================"

# 仅编译 (跳过后处理步骤) - 适用于快速重编译
build: directories $(TARGET)

# 后处理步骤 - 运行 Windows 批处理文件
postbuild: $(TARGET) $(BIN_DIR)/appxm.o
	@echo "运行后处理步骤..."
	$(BIN_DIR)/postbuild.bat $(PROJECT_NAME)

# 创建输出目录
directories:
	mkdir -p $(OBJ_DIR)/driver
	mkdir -p $(OBJ_DIR)/hardware
	mkdir -p $(BIN_DIR)

# 链接
$(TARGET): $(LINKER_SCR) $(OBJS)
	@echo "链接 $(TARGET)..."
	$(LD) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
	@echo "输出文件: $@"

# 从 ram.ld 生成链接脚本
$(LINKER_SCR): ram.ld | directories
	@echo "处理链接脚本..."
	$(CC) $(CFLAGS) $(INCLUDES) -E -P -x c -c $< -o $@

# 处理 app.xm
$(BIN_DIR)/appxm.o: $(BIN_DIR)/app.xm
	@echo "处理 app.xm..."
	$(CC) $(CFLAGS) $(INCLUDES) -E -P -x c -c $< -o $@

# ============================================================================
# 编译规则
# ============================================================================

# 驱动编译规则
$(OBJ_DIR)/driver/%.o: $(DRIVER_DIR)/%.c | directories
	@echo "编译 $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 硬件抽象层编译规则
$(OBJ_DIR)/hardware/%.o: hardware/%.c | directories
	@echo "编译 $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 主程序编译规则
$(OBJ_DIR)/%.o: %.c | directories
	@echo "编译 $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# ============================================================================
# 清理
# ============================================================================

clean:
	@echo "清理中..."
	rm -rf $(OBJ_DIR)
	rm -f $(BIN_DIR)/app.rv32
	rm -f $(BIN_DIR)/app.bin
	rm -f $(BIN_DIR)/app.dcf
	rm -f $(BIN_DIR)/map.txt
	rm -f $(BIN_DIR)/appxm.o
	@echo "清理完成。"

# ============================================================================
# 重新构建
# ============================================================================

rebuild: clean all

# ============================================================================
# 帮助信息
# ============================================================================

help:
	@echo "用法: make [目标]"
	@echo ""
	@echo "可用目标:"
	@echo "  all      - 完整构建，包含后处理 (默认)"
	@echo "  build    - 仅编译链接 (跳过批处理脚本)"
	@echo "  clean    - 清理所有编译产物"
	@echo "  rebuild  - 清理后重新完整构建"
	@echo "  help     - 显示此帮助信息"
	@echo ""
	@echo "项目名称: $(PROJECT_NAME)"
	@echo "输出文件: $(TARGET)"
