#ifndef RTC_H_
#define RTC_H_

#include "twi.h"

#define RTC_ADDR_W 0xa2
#define RTC_ADDR_R 0xa3

#define RTC_CTRL_STAT_1 0x00
#define RTC_CTRL_STAT_2 0x01
#define RTC_SECOND 0x02
#define RTC_MINUTE 0x03
#define RTC_HOUR 0x04
#define RTC_DAY 0x05
#define RTC_WEEKDAY 0x06
#define RTC_MONTH 0x07
#define RTC_YEAR 0x08
#define RTC_ALARM_MINUTE 0x09
#define RTC_ALARM_HOUR 0x0a
#define RTC_ALARM_DAY 0x0b
#define RTC_ALARM_WEEKDAY 0x0c
#define RTC_CLKOUT_CTRL 0x0d
#define RTC_TIMER_CTRL 0x0e
#define RTC_TIMER 0x0f

//BITS
#define RTC_CLKOUT_BIT 7
#define RTC_CLKOUT_FD1_BIT 1
#define RTC_CLKOUT_FD0_BIT 0

//CONVERSION
#define dec2bcd(x) ((( (x)/10)<<4) + ((x) % 10))
#define bcd2dec(x) ((( (x)>>4)*10) + ((x) & 0x0f))

struct datetime {
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t day;
    uint8_t weekday;
    uint8_t month;
    uint8_t year;
};

void rtc_init(void);
void rtc_set_date_time(struct datetime*);
void rtc_get_date_time(struct datetime*);

#endif