#include "include.h"
#include "driver_com.h"
#include "driver_clk.h"
#include "driver_rtc.h"
#include "driver_gpio.h"
#include "rtc_calendar.h"

#define RTC_LED_PORT                    GPIOB_REG
#define RTC_LED_PIN                     GPIO_PIN_0


// 判断year为闰年的条件
#define LEAP_YEAR_CONDITION(year)       (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))

// 星期英文简写
const char *weekday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sta"};

// 时间、日期结构体
volatile rtc_time_typedef time_struct;
volatile rtc_date_typedef date_struct;

// 闹钟测试输出
AT(.com_rodata.isr)
const char alarm_tip[] = "\n----------------->> alarm <<--------------------\n\n";

// 中断服务：闹钟中断
AT(.com_text.isr)
void rtc_isr(void)
{
    if (rtc_get_flag(RTC_IT_ALM) != RESET) {
        rtc_clear_flag(RTC_IT_ALM);
        printf(alarm_tip);
    }
}

// 设置闹钟时间（24H），同时开启中断
void set_alarm(uint8_t hour, uint8_t minute)
{
    uint32_t current_timestamp;
    uint32_t alarm_timestamp;
    uint32_t timestamp_offet;

    // check the time valid.
    if (hour > 23 || minute > 59) {
        return;
    }

    current_timestamp = rtc_get_counter();

    // get timestamp only including date.
    alarm_timestamp = current_timestamp - (current_timestamp % 86400);
    timestamp_offet = hour * 3600 + minute * 60;

    // time zone time conversion to UTC.
    if (TIME_ZONE < 13) {
        alarm_timestamp -= 3600 * TIME_ZONE;
    } else if (TIME_ZONE < 25) {
        alarm_timestamp += 3600 * TIME_ZONE;
    }

    // if the current time has exceeded the time point, set the alarm for tomorrow.
    if ((alarm_timestamp + timestamp_offet) <= current_timestamp) {
        alarm_timestamp += 86400;
    }
    alarm_timestamp += timestamp_offet;

    rtc_set_alarm(alarm_timestamp - 1);  //RTCALM = (real time) - 1.
    rtc_pic_config(rtc_isr, 0, RTC_IT_ALM, ENABLE);
}

// 设置日期信息
void set_date(uint16_t year, uint8_t mon, uint8_t day)
{
    uint32_t current_timestamp;
    uint32_t new_timestamp;
    uint32_t timestamp_offset;
    uint32_t tmp_day;
    u8 i;

    // check the year and month valid.
    if (year < 1970 || year > 2100 ||
        mon < 1 || mon > 12) {
        return;
    }

    // check the day valid.
    switch (mon) {
        case 4:
        case 6:
        case 9:
        case 11:
            if (day < 1 || day > 30) {
                return;
            }
            break;
        case 2:
            if (day < 1 || day > 29) {
                return;
            } else if (!LEAP_YEAR_CONDITION(year) && (day == 29)) {
                return;
            }
            break;
        default:
            if (day < 1 || day > 31) {
                return;
            }
    }

    // convert date to timestamp.
    current_timestamp = rtc_get_counter();
    timestamp_offset = current_timestamp % 86400;
    tmp_day = day - 1;

    for (i = 1; i < 13; i++) {
        if (i == mon) {
            break;
        }
        switch (i) {
            case 4:
            case 6:
            case 9:
            case 11:
                tmp_day += 30;
                break;
            case 2:
                if (LEAP_YEAR_CONDITION(year)) {
                    tmp_day += 29;
                } else {
                    tmp_day += 28;
                }
                break;
            default:
                tmp_day += 31;
        }
    }

    while (year > 1970) {
        year--;
        if (LEAP_YEAR_CONDITION(year)) {
            tmp_day += 366;
        } else {
            tmp_day += 365;
        }
    }

    // set the timestamp to register.
    new_timestamp = tmp_day * 86400 + timestamp_offset;
    rtc_set_counter(new_timestamp);
}

// 设置时间信息（24H）
void set_time(uint8_t hour, uint8_t minute, uint8_t second)
{
    uint32_t current_timestamp;
    uint32_t new_timestamp;
    uint32_t timestamp_offet;

    // check the time valid.
    if (hour > 23 || minute > 59 || second > 59) {
        return;
    }

    // modify the timestamp according to param.
    current_timestamp = rtc_get_counter();
    new_timestamp = current_timestamp - (current_timestamp % 86400);
    timestamp_offet = hour * 3600 + minute * 60 + second;
    new_timestamp += timestamp_offet;

    // can't set the time before (UTC)1970/1/1 00:00:00
    if (new_timestamp < 3600 * TIME_ZONE) {
        return;
    }

    // time zone time conversion to UTC.
    if (TIME_ZONE < 13) {
        new_timestamp -= 3600 * TIME_ZONE;
    } else if (TIME_ZONE < 25) {
        new_timestamp += 3600 * TIME_ZONE;
    }

    // set the time to register.
    rtc_set_counter(new_timestamp);
}

// 获取闹钟时间（24H）
void get_alarm(void)
{
    uint32_t alarm_timestamp;

    // get the timestamp from register.
    alarm_timestamp = rtc_get_alarm() + 1; //(real time) = RTCALM + 1.

    // UTC to time zone time.
    if (TIME_ZONE < 13) {
        alarm_timestamp += 3600 * TIME_ZONE;
    } else if (TIME_ZONE < 25) {
        alarm_timestamp -= 3600 * TIME_ZONE;
    }

    // write the time info to struct.
    time_struct.hour = alarm_timestamp % 86400 / 3600;
    time_struct.minute = alarm_timestamp % 86400 % 3600 / 60;
    time_struct.second = 0;
}

// 获取日期
void get_date(void)
{
    u32 now_unix;
    u16 year = 1970, day = 0, tmp_day = 0;
    u8 mon = 0;

    now_unix = rtc_get_counter();
    if (TIME_ZONE < 13) {
        now_unix += 3600 * TIME_ZONE;
    } else if (now_unix < 25) {
        now_unix -= 3600 * (TIME_ZONE - 12);
    }

    day = now_unix / 86400;

    while (day >= 365) {
        if (LEAP_YEAR_CONDITION(year)) {
            if (day <= 365) {
                break;
            }
            day -= 366;
        } else {
            day -= 365;
        }
        year++;
    }

    for (mon = 1; mon < 13; mon++) {
        tmp_day = day;
        switch (mon) {
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                day -= 31;
                break;
            case 4:
            case 6:
            case 9:
            case 11:
                day -= 30;
                break;
            case 2:
                if (LEAP_YEAR_CONDITION(year)) {
                    day -= 29;
                } else {
                    day -= 28;
                }
                break;
        }
        if (day > 365) {
            day = tmp_day + 1;
            break;
        }
    }

    date_struct.year = year;
    date_struct.month = mon;
    date_struct.day = day;

    if (mon == 1 || mon == 2) {
        mon += 12;
        year -= 1;
    }
    date_struct.weekday = (day + 2 * mon + 3 * (mon + 1) / 5 + year +
                           year / 4 - year / 100 + year / 400 + 1) % 7;
}

// 获取时间（24H）
void get_time(void)
{
    u32 now_unix;

    now_unix = rtc_get_counter();
    if (TIME_ZONE < 13) {
        now_unix += 3600 * TIME_ZONE;
    } else if (now_unix < 25) {
        now_unix -= 3600 * (TIME_ZONE - 12);
    }

    time_struct.hour = (now_unix % 86400) / 3600;
    time_struct.minute = (now_unix % 86400) % 3600 / 60;
    time_struct.second = (now_unix % 86400) %60;
}

// 初始化
void calendar_config(void)
{
    /* The RTCC module Freq_source: XOSC24M_DIV12 / 64 = 31250Hz */
    /* So, we should set the prescale to (31250 - 1) */
    /* Then we will get 1Hz clock theoretically */
    clk_clk32k_rtc_set(CLK_CLK32K_RTC_SNF_RC_RTC); //enable rtc clock
    u32 snf_rc2m_clk = rc2m_clk_nhz_get(0); //get snf_rc2m actual frequency
    rtc_prescale_set((snf_rc2m_clk/64)-1); //Config predivision factor for 1Hz counter

    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = RTC_LED_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_I_6MA;

    gpio_init(RTC_LED_PORT, &gpio_init_structure);
}

void rtc_calendar_init(void)
{
    calendar_config();

    // 设置时间
    set_date(2022, 8, 16);
    set_time(12, 0, 50);

    // 若该时间点已过，则延后一天
    set_alarm(12, 1);

    get_alarm();
    printf("\n------> set alarm: %02d:%02d:%02d\n\n", time_struct.hour, time_struct.minute, time_struct.second);
}

// 测试函数
void rtc_calendar_example(void)
{
    static u8 time_fresh_flag = 0;

    get_date();
    get_time();

    if (time_fresh_flag != time_struct.second) {
        time_fresh_flag = time_struct.second;
        printf("===> now time: %d-%02d-%02d %02d:%02d:%02d  %s\n",
               date_struct.year, date_struct.month, date_struct.day,
               time_struct.hour, time_struct.minute, time_struct.second,
               weekday[date_struct.weekday]);
               gpio_toggle_bits(RTC_LED_PORT, RTC_LED_PIN);
    }
}
