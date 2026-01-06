# PWM 车控照明程序流程说明

## 系统概述

本项目是基于 AB2039P RISC-V 32位微控制器的车控照明系统，通过低功耗蓝牙(BLE)实现远程控制，支持多种灯光模式。核心模块为 **pwm.c**，负责双色温LED灯的PWM控制。

---

## 1. 系统启动流程

### 1.1 主程序入口 ([main.c](project/ble_peripheral/main.c))

```
main()
  ├─> sys_rst_init()              // 系统复位初始化
  ├─> bsp_sys_init()              // 板级支持包初始化 ⭐
  ├─> prod_test_init()            // 产测初始化
  └─> func_run()                  // 功能主循环 ⭐
```

### 1.2 板级初始化 ([bsp_sys.c:bsp_sys_init()](project/ble_peripheral/bsp/bsp_sys.c#L154))

```
bsp_sys_init()
  ├─> bsp_var_init()              // 变量初始化
  ├─> bsp_saradc_init()           // ADC初始化(按键/电压检测)
  ├─> bsp_key_init()              // 按键初始化
  ├─> pmu_init()                  // 电源管理初始化
  ├─> sys_clk_set()               // 系统时钟设置(24MHz)
  ├─> bsp_param_init()            // 参数存储初始化
  ├─> xosc_init()                 // 外部晶振初始化
  ├─> pwm_init() ⭐⭐⭐            // PWM模块初始化(核心)
  ├─> sys_set_tmr_enable()        // 启用5ms定时器
  └─> func_cb.sta = FUNC_BT       // 设置进入蓝牙模式
```

---

## 2. PWM模块初始化详解 ([pwm.c:pwm_init()](project/ble_peripheral/bsp/pwm.c#L145))

### 2.1 硬件配置

#### GPIO配置
- **PWM0通道** (亮度控制): GPIO_PB3 → TIMERP_PWM0
- **PWM3通道** (色温控制): GPIO_PB4 → TIMERP_PWM3
- **驱动能力**: 6mA
- **初始状态**: GPIO输出低电平(灯灭)

#### PWM定时器配置
```c
时钟源:        XOSC 24MHz
时钟分频:      DIV_4 (单位时间: 4/24 = 0.167us)
PWM0频率:      10kHz (周期 600-1 = 100us)
PWM3频率:      1kHz  (周期 6000-1 = 1000us)
定时器模式:    G2模式(双频率输出)
初始占空比:    0% (灯灭状态)
```

### 2.2 数据结构初始化

#### 灯光控制结构体 (light_cb)
```c
light_cb.power = 1                    // 电源开关(1:开启)
light_cb.mode = LIGHT_MODE_STATIC     // 模式: 静态
light_cb.auto_weather_enable = 0      // 自动天气模式: 关闭
light_cb.brightness_target = 10000    // 目标亮度: 100%
light_cb.brightness_current = -1      // 当前亮度: 未设置
light_cb.temperature_target = 0       // 目标色温: 0(暖白)
light_cb.temperature_current = -1     // 当前色温: 未设置
light_cb.weather_index = 1            // 天气场景索引
```

### 2.3 参数持久化

首次启动时创建标记(PARAM_LIGHT_MARK = 0x55)，后续启动从Flash读取:
- 灯光模式 (PARAM_LIGHT_MODE)
- 自动天气开关 (PARAM_LIGHT_AUTO_WEATHER)
- 亮度值 (PARAM_LIGHT_BRIGHTNESS)
- 色温值 (PARAM_LIGHT_TEMPERATURE)
- 天气索引 (PARAM_LIGHT_WEATHER_INDEX)

---

## 3. 定时调度系统

### 3.1 定时器中断 ([bsp_sys.c:usr_tmr5ms_callback()](project/ble_peripheral/bsp/bsp_sys.c#L27))

```
系统5ms定时器中断
  ├─> bsp_saradc_scan()           // ADC扫描 (每5ms)
  ├─> bsp_key_scan()              // 按键扫描 (每5ms)
  ├─> pwm_driver_10ms_isr() ⭐    // PWM更新 (每10ms)
  ├─> lowpwr_tout_ticks()         // 低功耗超时 (每100ms)
  ├─> MSG_SYS_500MS               // 系统消息 (每500ms)
  └─> MSG_SYS_1S                  // 系统消息 (每1s)
```

### 3.2 PWM周期更新 ([pwm.c:pwm_driver_10ms_isr()](project/ble_peripheral/bsp/pwm.c#L443))

```
pwm_driver_10ms_isr() (10ms调用一次)
  └─> light_module_10ms_function() ⭐⭐⭐
```

---

## 4. 灯光模式控制核心 ([pwm.c:light_module_10ms_function()](project/ble_peripheral/bsp/pwm.c#L298))

### 4.1 模式分支

```
light_module_10ms_function()
  ├─> 检查: init状态 && power开关
  ├─> LIGHT_MODE_STATIC     → 静态模式
  ├─> LIGHT_MODE_FLASH      → 闪烁模式
  ├─> LIGHT_MODE_FADE       → 渐变模式
  ├─> LIGHT_MODE_SOS        → SOS闪灯模式
  ├─> LIGHT_MODE_WEATHER    → 天气情景模式
  └─> LIGHT_MODE_DRL        → 日间行车灯模式
```

### 4.2 各模式实现详解

#### 模式1: 静态模式 (LIGHT_MODE_STATIC)
```c
// 亮度调整
if (brightness_current != brightness_target) {
    brightness_current = brightness_target    // 直接设置到目标值
    TMRP_SET_BRIGHTNESS(brightness_current)   // 更新PWM占空比
}

// 色温调整
if (temperature_current != temperature_target) {
    temperature_current = temperature_target
    TMRP_SET_TEMPERATURE(temperature_current) // 更新PWM占空比
}
```
**特点**: 立即响应用户设置，无过渡动画

#### 模式2: 闪烁模式 (LIGHT_MODE_FLASH)
```
时序: 60ms周期 (每10ms调用，计数6次)
动效: 亮60ms → 灭60ms → 循环
色温切换: 暖白(0) → 冷白(10000) → 中性(5000) → 循环
亮度: 固定100%
```

#### 模式3: 渐变模式 (LIGHT_MODE_FADE)
```c
步进: 每10ms增减20个单位
范围: 0 → 9980 → 0 (循环)
渐变: 暖白 ←→ 冷白 持续循环
亮度: 固定100%
```

#### 模式4: SOS模式 (LIGHT_MODE_SOS)
```
摩尔斯码: ... --- ... (短短短 长长长 短短短)

S信号(冷白): 亮200ms → 灭200ms × 3次
间隔: 400ms
O信号(暖白): 亮500ms → 灭200ms × 3次
间隔: 400ms
S信号(冷白): 亮200ms → 灭200ms × 3次
循环间隔: 1000ms
```

#### 模式5: 天气情景模式 (LIGHT_MODE_WEATHER)
```
行为: 与静态模式相同
特点: 记录天气场景索引，便于自动切换
```

#### 模式6: 日间行车灯模式 (LIGHT_MODE_DRL)
```
亮度: 固定10% (1000/10000)
色温: 纯冷白 (10000)
```

### 4.3 PWM占空比设置宏

#### 亮度控制 (TMRP_SET_BRIGHTNESS)
```c
// brightness范围: 0~10000 (对应0%~100%)
duty0 = brightness * (period + 1) / 10000 - 1

// 特殊处理:
if (brightness == 0)
    GPIO_PB3设为GPIO模式 → 输出低电平 (彻底关闭)
else
    GPIO_PB3设为PWM模式
```

#### 色温控制 (TMRP_SET_TEMPERATURE)
```c
// temperature范围: 0~10000 (0=暖白, 10000=冷白)
real_pwm = 10000 - temperature  // 反转逻辑
duty3 = real_pwm * (g2period + 1) / 10000 - 1

// 特殊处理:
if (real_pwm == 0)
    GPIO_PB4设为GPIO模式 → 输出低电平
else
    GPIO_PB4设为PWM模式
```

---

## 5. 蓝牙控制接口

### 5.1 BLE通信流程

```
手机APP
  ↓ (BLE写入特征值)
[ble_user_service.c:service_write_callback()]
  ↓ (解析命令)
[pwm.c:light_module_ble_event()] ⭐
  ↓ (设置参数)
更新 light_cb 结构体
  ↓ (10ms后生效)
[light_module_10ms_function()]
  ↓ (硬件输出)
PWM波形 → LED灯光变化
```

### 5.2 BLE命令协议 ([pwm.c:light_module_ble_event()](project/ble_peripheral/bsp/pwm.c#L448))

#### 协议格式
所有命令以 `0x01 0xF9` 开头

#### 命令列表

| 命令 | 格式 | 功能 | 示例 |
|------|------|------|------|
| **同步状态** | `01 F9 01 03` | 返回当前所有状态 | 连接后首次同步 |
| **灯光开关** | `01 F9 02 01 [ON/OFF]` | 开关灯光 | `01 F9 02 01 01` 开灯 |
| **自动天气** | `01 F9 02 02 [ON/OFF]` | 开关自动天气模式 | `01 F9 02 02 00` 关闭 |
| **静态模式** | `01 F9 03 01 [色温] [亮度]` | 设置色温(0~100)和亮度(1~100) | `01 F9 03 01 32 64` 中性白50%亮度 |
| **天气模式** | `01 F9 03 02 [索引] [色温] [亮度]` | 场景索引+色温+亮度 | `01 F9 03 02 03 0 100` 晴天场景 |
| **闪烁模式** | `01 F9 03 03` | 启动三色循环闪烁 | - |
| **渐变模式** | `01 F9 03 04` | 启动色温渐变 | - |
| **SOS模式** | `01 F9 03 05` | 启动SOS求救信号 | - |
| **DRL模式** | `01 F9 03 06` | 日间行车灯 | - |

#### 响应格式
设备通过BLE通知(Notify)返回相同格式的数据确认

---

## 6. 主循环处理 ([func.c:func_process()](project/ble_peripheral/app/func.c#L8))

```
func_process() (主循环不断调用)
  ├─> WDT_CLR()                         // 喂狗
  ├─> bsp_vbat_process()                // 电池电压检测
  ├─> bsp_tsensor_process()             // 温度传感器
  ├─> light_mode_event() ⭐              // 灯光状态打印
  ├─> light_module_check_save_param_event() ⭐  // 参数保存
  └─> prod_test_process()               // 产测处理
```

### 6.1 状态监控 ([pwm.c:light_mode_event()](project/ble_peripheral/bsp/pwm.c#L546))

```c
// 串口打印当前亮度和色温(仅在参数变化时)
printf("curr brightness: %05d, temperature: %05d\n",
       light_cb.brightness_target,
       light_cb.temperature_target);
```

### 6.2 参数自动保存 ([pwm.c:light_module_check_save_param_event()](project/ble_peripheral/bsp/pwm.c#L102))

```
检测参数变化
  ├─> mode / auto_weather_enable / brightness / temperature / weather_index
  ├─> 启动1秒延迟计时器
  └─> 1秒后无新变化 → 写入Flash存储
      ├─> bsp_param_write(...)
      └─> bsp_param_sync()
```

**作用**: 防止频繁写Flash，延长寿命

---

## 7. 完整数据流图

```
┌─────────────────────────────────────────────────────────────┐
│                      系统启动                               │
│  main() → sys_rst_init() → bsp_sys_init() → func_run()   │
└──────────────────────┬──────────────────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────────────┐
│                   pwm_init() 初始化                         │
│  • 配置GPIO: PB3(PWM0), PB4(PWM3)                           │
│  • 设置定时器: 10kHz / 1kHz                                 │
│  • 读取Flash参数 → light_cb结构体                          │
│  • 初始状态: 灯灭(占空比0%)                                 │
└──────────────────────┬──────────────────────────────────────┘
                       │
        ┌──────────────┴──────────────┐
        ▼                             ▼
┌───────────────────┐         ┌──────────────────┐
│  5ms定时器中断    │         │  BLE事件触发     │
│  usr_tmr5ms_      │         │  service_write_  │
│  callback()       │         │  callback()      │
└────────┬──────────┘         └────────┬─────────┘
         │ (每10ms)                    │
         ▼                             ▼
┌───────────────────┐         ┌──────────────────┐
│ pwm_driver_10ms_  │         │ light_module_    │
│ isr()             │         │ ble_event()      │
└────────┬──────────┘         └────────┬─────────┘
         │                             │
         │         ┌───────────────────┘
         │         │ (设置目标参数)
         ▼         ▼
┌─────────────────────────────────────────┐
│  light_module_10ms_function()           │
│  • 读取 light_cb.mode                   │
│  • 执行对应模式逻辑                     │
│  • 更新 current → target               │
│  • 调用 TMRP_SET_BRIGHTNESS/TEMPERATURE │
└──────────────────┬──────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────┐
│  硬件PWM输出                            │
│  PWM0 → LED亮度通道                    │
│  PWM3 → LED色温通道                    │
└──────────────────┬──────────────────────┘
                   │
                   ▼
           ┌──────────────┐
           │  双色温LED灯 │
           │  实时响应    │
           └──────────────┘

┌─────────────────────────────────────────┐
│  主循环 (func_process)                  │
│  • light_mode_event() 状态打印          │
│  • light_module_check_save_param_event()│
│    参数变化1秒后写入Flash               │
└─────────────────────────────────────────┘
```

---

## 8. 关键时序参数

| 项目 | 数值 | 说明 |
|------|------|------|
| **系统时钟** | 24MHz | XOSC晶振 |
| **定时器时钟** | 6MHz | 24MHz ÷ 4 |
| **PWM0频率** | 10kHz | 亮度通道(避免频闪) |
| **PWM3频率** | 1kHz | 色温通道 |
| **控制周期** | 10ms | 模式更新周期 |
| **参数保存延迟** | 1000ms | 防抖动保护 |
| **闪烁周期** | 60ms | 亮/灭切换 |
| **渐变步进** | 20/10ms | 每10ms变化0.2% |
| **SOS短信号** | 200ms | 摩尔斯码点 |
| **SOS长信号** | 500ms | 摩尔斯码划 |

---

## 9. 文件依赖关系

```
pwm.c (核心模块)
  ├─ 引用头文件:
  │   ├─ driver_pwm.h          // PWM驱动API
  │   ├─ driver_gpio.h         // GPIO驱动API
  │   └─ ble_user_service.h    // BLE服务通知接口
  │
  ├─ 被调用位置:
  │   ├─ bsp_sys.c:191         // pwm_init()
  │   ├─ bsp_sys.c:41          // pwm_driver_10ms_isr()
  │   ├─ func.c:20-21          // light_mode_event() + check_save
  │   └─ ble_user_service.c:112 // light_module_ble_event()
  │
  └─ 调用接口:
      ├─ tmrp_*() 系列         // 定时器PWM底层驱动
      ├─ gpio_*() 系列         // GPIO配置函数
      ├─ bsp_param_*()         // Flash参数存储
      └─ service_notify_event() // BLE通知发送
```

---

## 10. 调试要点

### 10.1 串口日志关键输出

```c
// 初始化完成
"bsp_sys_init"

// 参数变化时
"curr brightness: XXXXX, temperature: XXXXX"

// 按键事件
"PP key short key up"
```

### 10.2 常见问题排查

| 问题 | 可能原因 | 检查点 |
|------|----------|--------|
| 灯不亮 | power=0 | 检查 light_cb.power |
| 亮度无法调节 | mode错误 | 确认处于STATIC/WEATHER模式 |
| 色温无法调节 | GPIO配置 | 检查PB4引脚功能 |
| 闪烁异常 | 定时器未启动 | 确认sys_set_tmr_enable(1,1) |
| 参数不保存 | 延迟未到 | 等待1秒后断电测试 |
| BLE控制无响应 | 协议错误 | 检查命令头0x01 0xF9 |

---

## 11. 扩展建议

### 可优化方向

1. **渐变优化**: 启用 `PWM_CHANGE_FADE_ENABLE` 实现平滑亮度过渡(变速渐变算法已实现)
2. **自定义模式**: 在 BLE协议中增加自定义闪烁/渐变参数
3. **功耗优化**: 在灯灭状态关闭PWM时钟
4. **故障检测**: 添加LED短路/开路检测(通过ADC监测驱动电流)
5. **温度保护**: 集成温度传感器实现过热降亮度

---

## 12. 总结

### 核心执行路径

```
pwm_init() 初始化
    ↓
10ms定时器触发 pwm_driver_10ms_isr()
    ↓
light_module_10ms_function() 根据模式更新PWM
    ↓
TMRP_SET_BRIGHTNESS / TMRP_SET_TEMPERATURE 设置占空比
    ↓
硬件PWM输出 → LED灯光变化
```

### 设计亮点

1. **双频PWM**: 10kHz消除频闪，1kHz降低功耗
2. **零占空比优化**: 切换GPIO模式实现真正关闭
3. **防抖动保存**: 1秒延迟写Flash
4. **模式化设计**: 6种灯光模式独立实现
5. **BLE实时控制**: 协议简洁，响应快速(10ms级)

---

**文档版本**: v1.0
**适用固件**: AB2039P BLE照明控制
**最后更新**: 2026-01-02
