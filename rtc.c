#include "rtc.h"

void rtc_init(void){
    I2C_start();
    I2C_write(RTC_ADDR_W); //Enable 1 second pulses
    I2C_write(RTC_CLKOUT_CTRL); // 0x0d
    I2C_write( (1<<RTC_CLKOUT_BIT)|(1<<RTC_CLKOUT_FD0_BIT)|(1<<RTC_CLKOUT_FD1_BIT));
    I2C_stop(); // (1<<7) | (1<<1) | (1<<0)
}

void rtc_set_date_time(struct datetime *dt){
    I2C_start();
    I2C_write(RTC_ADDR_W);
    I2C_write(RTC_SECOND);
    I2C_write(dec2bcd(dt->sec));
    I2C_write(dec2bcd(dt->min));
    I2C_write(dec2bcd(dt->hour));
    I2C_write(dec2bcd(dt->day));
    I2C_write(dec2bcd(dt->weekday));
    I2C_write(dec2bcd(dt->month));
    I2C_write(dec2bcd(dt->year));
    I2C_stop();
}

void rtc_get_date_time(struct datetime *dt){
    uint8_t v;
    I2C_start();
    I2C_write(RTC_ADDR_W);
    I2C_write(RTC_SECOND);
    I2C_start();
    I2C_write(RTC_ADDR_R);
    I2C_read(&v, ACK);
    dt->sec = bcd2dec(v &       0b01111111);
    I2C_read(&v, ACK);
    dt->min = bcd2dec(v &       0b01111111);
    I2C_read(&v, ACK);
    dt->hour = bcd2dec(v &      0b00111111);
    I2C_read(&v, ACK);
    dt->day = bcd2dec(v &       0b00111111);
    I2C_read(&v, ACK);
    dt->weekday = bcd2dec(v &   0b00000111);
    I2C_read(&v, ACK);
    dt->month = bcd2dec(v &     0b00011111);
    I2C_read(&v, NO_ACK);
    dt->year = bcd2dec(v);
    I2C_stop();
}