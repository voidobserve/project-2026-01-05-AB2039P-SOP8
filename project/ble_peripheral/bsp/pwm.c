#include "include.h"
#include "driver_pwm.h"
#include "driver_gpio.h"
#include "pwm.h"
#include "ble_user_service.h"
#include "user_config.h"

#if USER_DEBUG_ENABLE
#define TIMERP_PWM0_PORT GPIOB_REG
#define TIMERP_PWM0_PIN GPIO_PIN_2 /* 使用硬件上没有引出的引脚 */
#define TIMERP_PWM3_PORT GPIOB_REG
#define TIMERP_PWM3_PIN GPIO_PIN_4
#else
#define TIMERP_PWM0_PORT GPIOB_REG
#define TIMERP_PWM0_PIN GPIO_PIN_3
#define TIMERP_PWM3_PORT GPIOB_REG
#define TIMERP_PWM3_PIN GPIO_PIN_4
#endif

// #if BSP_UART_DEBUG_EN == GPIO_PB3
// #undef TIMERP_PWM0_PIN
// #define TIMERP_PWM0_PIN                     GPIO_PIN_5
// #endif

#define PWM_CHANGE_FADE_ENABLE 0
#define TEMPERATURE_RANGE_MAX (10000)
#define TMRP_SET_BRIGHTNESS(brightness)                                        \
    do                                                                         \
    {                                                                          \
        uint16_t real_pwm = brightness;                                        \
        TMRP->duty0 = (real_pwm) * ((uint32_t)(TMRP->period) + 1) / 10000 - 1; \
        if (real_pwm == 0)                                                     \
            TIMERP_PWM0_PORT->fen &= ~TIMERP_PWM0_PIN;                         \
        else                                                                   \
            TIMERP_PWM0_PORT->fen |= TIMERP_PWM0_PIN;                          \
        main_loop_printf_update = 1;                                           \
    } while (0)

#define TMRP_SET_TEMPERATURE(temperature)                                        \
    do                                                                           \
    {                                                                            \
        uint16_t real_pwm = 10000 - temperature;                                 \
        TMRP->duty3 = (real_pwm) * ((uint32_t)(TMRP->g2period) + 1) / 10000 - 1; \
        if (real_pwm == 0)                                                       \
            TIMERP_PWM3_PORT->fen &= ~TIMERP_PWM3_PIN;                           \
        else                                                                     \
            TIMERP_PWM3_PORT->fen |= TIMERP_PWM3_PIN;                            \
        main_loop_printf_update = 1;                                             \
    } while (0)

enum
{
    LIGHT_MODE_STATIC = 0x00,     // 静态
    LIGHT_MODE_AUTO,              // 自动模式
    LIGHT_MODE_DRL,               // 日间行车灯
    LIGHT_MODE_FLASH,             // 闪烁/爆闪
    LIGHT_MODE_FADE,              // 渐变
    LIGHT_MODE_SOS,               // SOS闪灯
    LIGHT_MODE_WEATHER,           // 天气模式
    LIGHT_MODE_WHITE = 16,        // 亮度不变，白光变为100%
    LIGHT_MODE_WHITE_YELLOW = 17, // 亮度不变，白光和黄光各自占50%
    LIGHT_MODE_YELLOW = 18,       // 亮度不变，黄光变为100%
};

typedef struct _light_module_t
{
    u8 init;
    u8 power; // 开关标志
    u8 mode;
    u8 auto_weather_enable;
    u16 brightness_current;
    u16 brightness_target;
#if PWM_CHANGE_FADE_ENABLE
    u16 brightness_step;
#endif
    u16 temperature_current;
    u16 temperature_target;
    u8 weather_index;

    u8 timeing_step;
    u8 timeing_state;
    u16 timeing;
    u16 timeing_c;

    u8 password_buff[6];
} light_module_t;

typedef struct _light_save_param_t
{
    u8 first_pwr_on_crc;
    u8 mode;
    // u8 auto_weather_enable;
    u16 brightness_target;
    u16 temperature_target;
    // u8 weather_index;
    u8 password_buff[6];
} light_save_param_t;

static light_module_t light_cb AT(.buf.light_cb);
static light_save_param_t light_save_param AT(.buf.light_cb);
static u8 main_loop_printf_update AT(.buf.light_cb);

AT(.com_text.isr)
void pwm_isq(void)
{
    if (tmrp_get_flag(TMRP, TMRP_TIE) != RESET)
    {
        tmrp_clear_flag(TMRP, TMRP_TIE);
    }
}

AT(.com_text.isr)
static u32 tick_get_nonzero(void)
{
    u32 ticks = tick_get();
    if (ticks == 0)
        ticks++;
    return ticks;
}

void light_module_check_save_param_event(void)
{
    static u32 ticks = 0;

    if (light_save_param.mode != light_cb.mode)
    {
        light_save_param.mode = light_cb.mode;
        ticks = tick_get_nonzero();
    }
    // if (light_save_param.auto_weather_enable != light_cb.auto_weather_enable)
    // {
    //     light_save_param.auto_weather_enable = light_cb.auto_weather_enable;
    //     ticks = tick_get_nonzero();
    // }
    if (light_save_param.brightness_target != light_cb.brightness_target)
    {
        light_save_param.brightness_target = light_cb.brightness_target;
        ticks = tick_get_nonzero();
    }
    if (light_save_param.temperature_target != light_cb.temperature_target)
    {
        light_save_param.temperature_target = light_cb.temperature_target;
        ticks = tick_get_nonzero();
    }
    // if (light_save_param.weather_index != light_cb.weather_index)
    // {
    //     light_save_param.weather_index = light_cb.weather_index;
    //     ticks = tick_get_nonzero();
    // }

    if (memcmp(light_save_param.password_buff, light_cb.password_buff, sizeof(light_save_param.password_buff)))
    {
        // 如果密码有修改
        memcpy((void *)light_save_param.password_buff, (const void *)light_cb.password_buff, sizeof(light_save_param.password_buff));
        ticks = tick_get_nonzero();
    }

    if (ticks && tick_check_expire(ticks, 1000))
    {
        ticks = 0;

        // bsp_param_write((u8 *)&light_save_param.mode, PARAM_LIGHT_MODE, sizeof(light_save_param.mode));
        // bsp_param_write((u8 *)&light_save_param.auto_weather_enable, PARAM_LIGHT_AUTO_WEATHER, sizeof(light_save_param.auto_weather_enable));
        // bsp_param_write((u8 *)&light_save_param.brightness_target, PARAM_LIGHT_BRIGHTNESS, sizeof(light_save_param.brightness_target));
        // bsp_param_write((u8 *)&light_save_param.temperature_target, PARAM_LIGHT_TEMPERATURE, sizeof(light_save_param.temperature_target));
        // bsp_param_write((u8 *)&light_save_param.weather_index, PARAM_LIGHT_WEATHER_INDEX, sizeof(light_save_param.weather_index));

        bsp_param_write((u8 *)&light_save_param, PARAM_LIGHT_START_ADDR, sizeof(light_save_param_t));

        bsp_param_sync();

        printf("light_save_param save\n");
    }
}

void pwm_init(void)
{
    gpio_init_typedef gpio_init_structure;
    tmrp_base_init_typedef tmrp_base_init_struct;
    u8 first = 0;

    light_cb.init = 0;

    clk_gate2_cmd(CLK_GATE2_TMRP, CLK_EN);

    /*****************************初始化配置**********************************/
    gpio_init_structure.gpio_pin = TIMERP_PWM0_PIN | TIMERP_PWM3_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    // gpio_init_structure.gpio_fen = GPIO_FEN_PER;  // 脚位特殊功能，PWM时需要设置成这个
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO; // 通用输入输出，由于PWM的占空比设置成0时无法彻底关闭，所以需要在占空比为0时设置成GPIO+输出低电平
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_I_6MA;
    gpio_init(TIMERP_PWM0_PORT, &gpio_init_structure);
    gpio_reset_bits(TIMERP_PWM0_PORT, TIMERP_PWM0_PIN); // 提前将GPIO设置成输出低电平，在设置PWM占空比为0时直接修改脚位FEN为GPIO即可
    gpio_reset_bits(TIMERP_PWM3_PORT, TIMERP_PWM3_PIN); // 提前将GPIO设置成输出低电平，在设置PWM占空比为0时直接修改脚位FEN为GPIO即可

    gpio_func_mapping_config(TIMERP_PWM0_PORT, TIMERP_PWM0_PIN, GPIO_CROSSBAR_OUT_PWM0);
    gpio_func_mapping_config(TIMERP_PWM3_PORT, TIMERP_PWM3_PIN, GPIO_CROSSBAR_OUT_PWM3);

    tmrp_base_init_struct.clock_source = TMRP_XOSC24M_CLK; // 24MHz
    tmrp_base_init_struct.clock_div = TMRP_CLOCK_DIV_4;    // div 4,  (4/24)us
    tmrp_base_init_struct.period = -1;                     // pwm 频率1，0
    tmrp_base_init_struct.g2_period = -1;                  // pwm 频率2，0
    tmrp_base_init_struct.tmr_mode = TMRP_TMR_MODE_G2;     // G2模式时g2_period才有效
    tmrp_base_init_struct.tick_mode = TMRP_TICK_MODE_UNUSED;
    tmrp_base_init_struct.pwm_num = TMRP_PWM0 | TMRP_PWM3;
    tmrp_base_init_struct.pwm_inv = TMRP_PWM_INV_UNUSED;
    tmrp_base_init_struct.start_duty0 = -1; // duty  0
    tmrp_base_init_struct.start_duty3 = -1; // duty  0
    tmrp_base_init_struct.dz_en = DISABLE;  // 死区, 0
    tmrp_base_init_struct.dzmode = TMRP_DZMODE_0;
    tmrp_base_init_struct.dzlen = 0;

    tmrp_base_init(TMRP, &tmrp_base_init_struct);
    // tmrp_pic_config(TMRP, pwm_isq, 0, TMRP_TIE, ENABLE);
    tmrp_cmd(TMRP, ENABLE);

    /*****************************随用随开**********************************/
    tmrp_set_period(TMRP, 600 - 1);    // period 100us-10khz
    tmrp_set_g2period(TMRP, 6000 - 1); // period 1000us-1khz

    tmrp_open_pwm(TMRP, TMRP_PWM0, TMRP_PWM_INV_UNUSED);
    tmrp_set_duty(TMRP, TMRP_PWM0, 0 - 1); // duty 0%

    tmrp_open_pwm(TMRP, TMRP_PWM3, TMRP_PWM_INV_UNUSED);
    tmrp_set_duty(TMRP, TMRP_PWM3, 0 - 1); // duty 0%

    light_cb.power = 1;
    light_cb.mode = LIGHT_MODE_STATIC;
    light_cb.auto_weather_enable = 0;
    light_cb.brightness_current = -1;
    light_cb.brightness_target = 10000;
    light_cb.temperature_current = -1;
    light_cb.temperature_target = 0;
    light_cb.weather_index = 1;
#if PWM_CHANGE_FADE_ENABLE
    light_cb.brightness_step = 0;
#endif
    light_cb.timeing = 0;
    light_cb.timeing_step = 0;
    light_cb.timeing_state = 1;
    light_cb.timeing_c = 0;

    // bsp_param_read((u8 *)&first, PARAM_LIGHT_MARK, sizeof(first));
    // if (first != 0x55)
    // {
    //     first = 0x55;
    //     bsp_param_write((u8 *)&first, PARAM_LIGHT_MARK, sizeof(first));

    //     bsp_param_write((u8 *)&light_cb.mode, PARAM_LIGHT_MODE, sizeof(light_cb.mode));
    //     bsp_param_write((u8 *)&light_cb.auto_weather_enable, PARAM_LIGHT_AUTO_WEATHER, sizeof(light_cb.auto_weather_enable));
    //     bsp_param_write((u8 *)&light_cb.brightness_target, PARAM_LIGHT_BRIGHTNESS, sizeof(light_cb.brightness_target));
    //     bsp_param_write((u8 *)&light_cb.temperature_target, PARAM_LIGHT_TEMPERATURE, sizeof(light_cb.temperature_target));
    //     bsp_param_write((u8 *)&light_cb.weather_index, PARAM_LIGHT_WEATHER_INDEX, sizeof(light_cb.weather_index));
    //     bsp_param_sync();
    // }
    // else
    // {
    //     bsp_param_read((u8 *)&light_cb.mode, PARAM_LIGHT_MODE, sizeof(light_cb.mode));
    //     bsp_param_read((u8 *)&light_cb.auto_weather_enable, PARAM_LIGHT_AUTO_WEATHER, sizeof(light_cb.auto_weather_enable));
    //     bsp_param_read((u8 *)&light_cb.brightness_target, PARAM_LIGHT_BRIGHTNESS, sizeof(light_cb.brightness_target));
    //     bsp_param_read((u8 *)&light_cb.temperature_target, PARAM_LIGHT_TEMPERATURE, sizeof(light_cb.temperature_target));
    //     bsp_param_read((u8 *)&light_cb.weather_index, PARAM_LIGHT_WEATHER_INDEX, sizeof(light_cb.weather_index));
    // }
    // light_save_param.mode = light_cb.mode;
    // light_save_param.auto_weather_enable = light_cb.auto_weather_enable;
    // light_save_param.brightness_target = light_cb.brightness_target;
    // light_save_param.temperature_target = light_cb.temperature_target;
    // light_save_param.weather_index = light_cb.weather_index;

    bsp_param_read((u8 *)&light_save_param, PARAM_LIGHT_START_ADDR, sizeof(light_save_param_t));
    if (light_save_param.first_pwr_on_crc != 0x55)
    {
        // 是第一次上电
        const u8 buff[] = {6, 6, 6, 6, 6, 6};

        printf("first power on\n");
        light_save_param.first_pwr_on_crc = 0x55;
        light_save_param.mode = light_cb.mode;
        light_save_param.brightness_target = light_cb.brightness_target;
        light_save_param.temperature_target = light_cb.temperature_target;
        memcpy((void *)light_save_param.password_buff, (const void *)buff, sizeof(light_save_param.password_buff));
        bsp_param_write((u8 *)&light_save_param, PARAM_LIGHT_START_ADDR, sizeof(light_save_param_t));
    }
    else
    {
        // 不是第一次上电
        printf("not first power on\n");
        bsp_param_read((u8 *)&light_save_param, PARAM_LIGHT_START_ADDR, sizeof(light_save_param_t));
    }

    // 初始化数据：
    light_cb.mode = light_save_param.mode;
    light_cb.brightness_target = light_save_param.brightness_target;
    light_cb.temperature_target = light_save_param.temperature_target;
    memcpy((void *)light_cb.password_buff, (const void *)light_save_param.password_buff, sizeof(light_save_param.password_buff));

    printf("light_cb.mode == %2d\n", light_cb.mode);
    printf("light_cb.brightness_target == %u\n", light_cb.brightness_target);
    printf("light_cb.temperature_target == %u\n", light_cb.temperature_target);
    {
        u8 i = 0;
        printf("-------------\n");
        printf("light_save_param.password_buff ==\n");
        for (i = 0; i < sizeof(light_save_param.password_buff); i++)
        {
            printf("%02x ", (int)light_save_param.password_buff[i]);
        }
        printf("\n-------------^\n");
    }

    light_cb.init = 1;
    main_loop_printf_update = 1;
}

/**
 * @brief 切换目标亮度，会自动切换到目标亮度
 */
AT(.com_text.isr)
void light_module_set_bright(u16 p_liangud)
{
    light_cb.brightness_target = p_liangud * 100;

#if PWM_CHANGE_FADE_ENABLE
    light_cb.brightness_step = 10;
#endif
}

/**
 * @brief 切换目标色温，会自动切换到目标色温
 */
AT(.com_text.isr)
void light_module_set_temperature(u16 p_wendu)
{
    light_cb.temperature_target = p_wendu * 100;
}

// SOS双色灯动效: ... --- ... (短短短 长长长 短短短)
// 短信号(S): 冷白色, 200ms亮 + 200ms灭
// 长信号(O): 暖白色, 500ms亮 + 200ms灭
// 字母间隔: 400ms, SOS循环间隔: 1000ms
AT(.com_rodata.pwm)
static const u8 sos_duration[] = {
    20, 20, 20, 20, 20, 20, // S: 亮灭亮灭亮灭 (各200ms)
    40,                     // 字母间隔 400ms
    50, 20, 50, 20, 50, 20, // O: 亮灭亮灭亮灭 (长信号500ms, 间隔200ms)
    40,                     // 字母间隔 400ms
    20, 20, 20, 20, 20, 20, // S: 亮灭亮灭亮灭 (各200ms)
    100                     // SOS循环间隔 1000ms
};
AT(.com_rodata.pwm)
static const u8 sos_state[] = {
    1, 0, 1, 0, 1, 0, // S: 亮灭亮灭亮灭
    0,                // 字母间隔 (灭)
    1, 0, 1, 0, 1, 0, // O: 亮灭亮灭亮灭
    0,                // 字母间隔 (灭)
    1, 0, 1, 0, 1, 0, // S: 亮灭亮灭亮灭
    0                 // SOS循环间隔 (灭)
};
AT(.com_rodata.pwm)
static const u16 sos_temperature[] = {
    10000,
    10000,
    10000,
    10000,
    10000,
    10000, // S: 冷白色
    10000, // 间隔
    0,
    10000,
    0,
    10000,
    0,
    10000, // O: 暖白色
    10000, // 间隔
    10000,
    10000,
    10000,
    10000,
    10000,
    10000, // S: 冷白色
    10000, // 间隔
};
AT(.com_text.isr)
void light_module_10ms_function(void)
{
    if (!light_cb.init || !light_cb.power)
        return;

    if (light_cb.mode == LIGHT_MODE_STATIC || light_cb.mode == LIGHT_MODE_WEATHER)
    {
        if (light_cb.brightness_current != light_cb.brightness_target)
        {
#if PWM_CHANGE_FADE_ENABLE
            if ((light_cb.brightness_current + light_cb.brightness_step) < light_cb.brightness_target)
            {
                light_cb.brightness_current += light_cb.brightness_step;
            }
            else if (light_cb.brightness_current > (light_cb.brightness_target + light_cb.brightness_step))
            {
                light_cb.brightness_current -= light_cb.brightness_step;
            }
            else
#endif
            {
                light_cb.brightness_current = light_cb.brightness_target;
            }

            TMRP_SET_BRIGHTNESS(light_cb.brightness_current); // 亮度
        }

        if (light_cb.temperature_current != light_cb.temperature_target)
        {
            light_cb.temperature_current = light_cb.temperature_target;

            TMRP_SET_TEMPERATURE(light_cb.temperature_current); // 色温
        }

#if PWM_CHANGE_FADE_ENABLE
        // 变速渐变
        if (light_cb.brightness_step < 50)
            light_cb.brightness_step += 1;
        if (light_cb.brightness_step >= 50)
            light_cb.brightness_step = 50;
#endif
    }
    else if (light_cb.mode == LIGHT_MODE_FLASH)
    {
        if (light_cb.timeing < 6)
        {
            light_cb.timeing++;
        }
        else
        {
            light_cb.timeing = 0;
            light_cb.timeing_state = !light_cb.timeing_state;
            if (light_cb.timeing_state)
            {
                light_cb.timeing_step++;
                light_cb.timeing_step %= 3;
            }

            TMRP_SET_BRIGHTNESS(10000);

            if (light_cb.timeing_state)
            {
                switch (light_cb.timeing_step)
                {
                case 0:
                    TMRP_SET_TEMPERATURE(0);
                    break;
                case 1:
                    TMRP_SET_TEMPERATURE(TEMPERATURE_RANGE_MAX);
                    break;
                case 2:
                    TMRP_SET_TEMPERATURE(TEMPERATURE_RANGE_MAX / 2);
                default:
                    break;
                }
            }
            else
            {
                TMRP_SET_BRIGHTNESS(0); // 低电平是亮
            }
        }
    }
    else if (light_cb.mode == LIGHT_MODE_FADE)
    {
        TMRP_SET_BRIGHTNESS(10000);

        if (light_cb.timeing_step == 0)
        {
            if (light_cb.timeing_c <= 9980)
            {
                light_cb.timeing_c += 20;
            }
            else
            {
                light_cb.timeing_step = 1;
            }
        }
        else // if (light_cb.timeing_step == 1)
        {
            if (light_cb.timeing_c >= 20)
            {
                light_cb.timeing_c -= 20;
            }
            else
            {
                light_cb.timeing_step = 0;
            }
        }
        // printf("timeing_c = %d\r\n",timeing_c);

        TMRP_SET_TEMPERATURE(light_cb.timeing_c);
    }
    else if (light_cb.mode == LIGHT_MODE_SOS)
    {
        if (light_cb.timeing_step >= (sizeof(sos_duration) / sizeof(sos_duration[0])))
            light_cb.timeing_step = 0;

        light_cb.timeing++;

        if (light_cb.timeing >= sos_duration[light_cb.timeing_step])
        {
            light_cb.timeing = 0;

            // 更新灯光状态
            if (sos_state[light_cb.timeing_step])
            {
                TMRP_SET_BRIGHTNESS(10000);
                TMRP_SET_TEMPERATURE(sos_temperature[light_cb.timeing_step]);
            }
            else
            {
                TMRP_SET_BRIGHTNESS(0);
            }

            light_cb.timeing_step++;
        }
    }
    else if (light_cb.mode == LIGHT_MODE_DRL)
    {
        TMRP_SET_BRIGHTNESS(1000);   // 亮度10%
        TMRP_SET_TEMPERATURE(10000); // 色温-纯白色
    }
}

AT(.com_text.isr)
void pwm_driver_10ms_isr(void)
{
    light_module_10ms_function();
}

void light_module_ble_event(u8 *buffer, u16 len)
{
    u16 param1; // 临时存放色温值
    u16 param2; // 临时存放亮度值
    if (buffer[0] != 0x01 || buffer[1] != 0xF9)
        return;

    if (len == 4 && buffer[2] == 0x01 && buffer[3] == 0x03)
    {
        // 同步
        service_notify_event((u8[]){0x01, 0xF9, 0x02, 0x01, light_cb.power}, 5);

        param1 = light_cb.temperature_target / 100;
        param2 = light_cb.brightness_target / 100;
        service_notify_event((u8[]){0x01, 0xF9, 0x03, 0x01, light_cb.mode, param1, param2}, 7);
    }
    else if (len == 5 && buffer[2] == 0x02 && buffer[3] == 0x01)
    {
        // 灯光开关
        // buffer[4](0:OFF,1:ON)
        light_cb.power = buffer[4] ? 1 : 0;
        service_notify_event((u8[]){0x01, 0xF9, 0x02, 0x01, light_cb.power}, 5);
        if (light_cb.power == 0)
        {
            TMRP_SET_BRIGHTNESS(0); // 亮度
            TMRP_SET_TEMPERATURE(0);
        }
        else
        {
            light_cb.brightness_current = -1;
            light_cb.temperature_current = -1;
        }
    }
    else if (len == 7 && buffer[2] == 0x03 && buffer[3] == 0x01)
    {
        /*
            设置模式、色温和亮度
            部分特殊的模式需要单片机一端来处理

        */
        u8 instruction = buffer[4];
        light_cb.mode = instruction;
        if (instruction == LIGHT_MODE_DRL)
        {
            light_cb.temperature_target = TEMPERATURE_RANGE_MAX; // 色温：纯白色
            light_cb.brightness_target = 1000;                   // 亮度10%
            light_cb.brightness_current = -1;
            light_cb.temperature_current = -1;
        }
        else if (instruction == LIGHT_MODE_FLASH)
        {
            // 不用改变亮度和色温，由扫描函数处理
        }
        else if (instruction == LIGHT_MODE_FADE)
        {
            // 不用改变亮度和色温，由扫描函数处理
        }
        else if (instruction == LIGHT_MODE_SOS)
        {
            // 不用改变亮度和色温，由扫描函数处理
        }
        else if (instruction == LIGHT_MODE_WHITE)
        {
            // 亮度不变，白光变为100%
            light_cb.temperature_target = TEMPERATURE_RANGE_MAX; // 色温：纯白色
            light_cb.temperature_current = -1;
        }
        else if (instruction == LIGHT_MODE_WHITE_YELLOW)
        {
            // 亮度不变，白光和黄光各自占50%
            light_cb.temperature_target = TEMPERATURE_RANGE_MAX / 2;
            light_cb.temperature_current = -1;
        }
        else if (instruction == LIGHT_MODE_YELLOW)
        {
            // 亮度不变，黄光变为100%
            light_cb.temperature_target = 0;
            light_cb.temperature_current = -1;
        }
        else
        {
            // 不是单片机控制的特殊模式，直接根据 app 发送过来的数据，调节色温值和亮度值
            param1 = buffer[5] <= 100 ? buffer[5] : 100;
            param2 = buffer[6] <= 100 ? buffer[6] : (buffer[6] >= 1 ? buffer[6] : 1);
            light_cb.temperature_target = param1 * 100;
            light_cb.brightness_target = param2 * 100;
            light_cb.brightness_current = -1;
            light_cb.temperature_current = -1;
        }

        param1 = light_cb.temperature_target / 100;
        param2 = light_cb.brightness_target / 100;
        service_notify_event((u8[]){0x01, 0xF9, 0x03, 0x01, light_cb.mode, param1, param2}, 7);

        printf("light mode == %02d\n", light_cb.mode);
    }
    else if (len == 10 && buffer[2] == 0x01 && buffer[3] == 0x01)
    {
        // 输入密码
        if (memcmp(light_cb.password_buff, &buffer[4], 6))
        {
            // 如果密码不一致
            service_notify_event((u8[]){0x01, 0xF9, 0x01, 0x04, 0x00}, 5);
        }
        else
        {
            // 密码一致
            service_notify_event((u8[]){0x01, 0xF9, 0x01, 0x04, 0x01}, 5);
        }
    }
    else if (len != 10 && buffer[2] == 0x01 && buffer[3] == 0x01)
    {
        // 输入密码，但是指令长度不一致
        service_notify_event((u8[]){0x01, 0xF9, 0x01, 0x04, 0x00}, 5);
    }
    else if (len == 10 && buffer[2] == 0x01 && buffer[3] == 0x02)
    {
        // 修改密码
        memcpy(light_cb.password_buff, &buffer[4], 6);
        service_notify_event((u8[]){0x01, 0xF9, 0x01, 0x05, 0x01}, 5); // 表示密码修改成功
    }
    else if (buffer[2] == 0x01 && buffer[3] == 0x02 && len != 10)
    {
        // 修改密码，但是指令长度不一致
        service_notify_event((u8[]){0x01, 0xF9, 0x01, 0x05, 0x00}, 5); // 表示密码修改失败
    }
}

void light_mode_event(void)
{
    if (main_loop_printf_update == 0)
        return;
    main_loop_printf_update = 0;

    if (LIGHT_MODE_DRL == light_cb.mode ||
        LIGHT_MODE_FLASH == light_cb.mode ||
        LIGHT_MODE_SOS == light_cb.mode)
    {
        // 测试时使用，不打印这些信息
        return;
    }

    printf("curr brightness: %05d, temperature: %05d\n", light_cb.brightness_target, light_cb.temperature_target);
    printf("light mode == %02d\n", light_cb.mode);
}