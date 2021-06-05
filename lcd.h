#ifndef _LCD_H
#define _LCD_H

/*
http://fizyka.if.pw.edu.pl/~labe/data/_uploaded/file/psm/Lab3.pdf
http://fizyka.if.pw.edu.pl/~labe/data/_uploaded/file/psm/materialy/Noty_katalogowe/Obsluga%20LCD.pdf
*/ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>

#ifndef LCD_PORT
#define LCD_PORT PORTA
#endif

#ifndef LCD_PORT_DIR
#define LCD_PORT_DIR DDRA
#endif

#ifndef LCD_E
#define LCD_E PA3
#endif

#ifndef LCD_RS
#define LCD_RS PA2
#endif

void lcdinit(void);
void lcd_write_command(uint8_t data);
void lcd_clear(void);
void lcd_write_data(uint8_t data);
void lcd_set_xy(uint8_t r, uint8_t k);
void lcd_write_text_xy(uint8_t r, uint8_t k, char *text);
void lcd_write_text_P(const char *text);

#endif