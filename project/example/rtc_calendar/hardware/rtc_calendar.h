#ifndef _RTC_CALENDAR_H
#define _RTC_CALENDAR_H


// time zone: utc: 0;  east[1:12]  west[13:24]
#define TIME_ZONE            8

// RTC time structure.
typedef struct {
    uint8_t     hour;
    uint8_t     minute;
    uint8_t     second;
} rtc_time_typedef;

// RTC date structure.
typedef struct {
    u16 year;
    u8  month;
    u8  day;
    u8  weekday;
}rtc_date_typedef;

void set_alarm(uint8_t hour, uint8_t minute);
void set_date(uint16_t year, uint8_t mon, uint8_t day);
void set_time(uint8_t hour, uint8_t minute, uint8_t second);
void get_alarm(void);
void get_date(void);
void get_time(void);
void calendar_config(void);
void rtc_calendar_init(void);
void rtc_calendar_example(void);

#endif // _RTC_CALENDAR_H
