/* File: automatic-blinds
*
* Created on: 06/2021
* Author: Krzysztof Palmi <01141448@pw.edu.pl>
*/
#ifndef __AVR_ATmega32__
	#define __AVR_ATmega32__
#endif

/*
Register TWBR = (16MHz / (100kHz) - 16)/2
---------------------------------------
Connection and usage:
https://www.overleaf.com/read/ncnhqmkssbnk
*/ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rs232.h"
#include "twi.h"
#include "rtc.h"
#include "lcd.h"

#define LED			PB0
#define SWITCH_EDIT PB3
#define SWITCH_UP 	PB4
#define SWITCH_DOWN PB5
#define SWITCH_NEXT PB6
#define SWITCH_SET 	PB7
#define SWITCH_BLINDS PC0

#define TURN_ON_LED		(PORTB &= ~(1<<LED))
#define TURN_OFF_LED	(PORTB |= (1<<LED))

#define BAUD 1000000
#define UBRR_VALUE (F_CPU/(16*BAUD)-1)

volatile uint8_t update_data_from_ISR = 1, check_buttons = 0, editing_mode = 0;
volatile uint8_t timer2_counter = 0;
volatile uint8_t blinds_editing_mode = 0, edited_blinds = 1;
volatile uint16_t shading_time = 0;
volatile uint8_t buffor_clear=0;
volatile uint8_t currently_shaded = 0;
volatile uint8_t service_shdtime = 20;	// multiply by 0,1s
volatile uint8_t service_mode = 0;

const uint16_t SHADING = 9546;	// 95,46s for 2m blinds

uint8_t currently_edited = 0;
uint8_t step = 0;
uint8_t direction = 0;
uint8_t motor_state = 0;

struct datetime current_date ={
	.sec = 	0,		// 5
	.min = 	0,		// 4
	.hour = 00,		// 3
	.day = 	01,		// 0
	.weekday = 01,
	.month = 01,	// 1
	.year = 21		// 2
};

struct datetime blinds_morning ={
	.sec = 	0,		
	.min = 	0,		
	.hour = 7,		
	.day = 	01,		// not used
	.weekday = 01,	// not used
	.month = 01,	// not used
	.year = 21		// not used
};

struct datetime blinds_night ={
	.sec = 	0,		
	.min = 	30,		
	.hour = 21,		
	.day = 	01,		// not used
	.weekday = 01,	// not used
	.month = 01,	// not used
	.year = 21		// not used
};

ISR(INT0_vect){ 	// set to 1s
	struct datetime dt;
	rtc_get_date_time(&dt);
	if(update_data_from_ISR){
		lcd_set_xy(0,0);
		printf("Date: %02d/%02d/20%02d", dt.day, dt.month, dt.year);
		lcd_set_xy(1,0);
		printf("Time: %02d:%02d:%02d", dt.hour, dt.min, dt.sec);
	}
	if(service_mode == 0){
		if(	(dt.hour == blinds_morning.hour) &
				(dt.min == blinds_morning.min) &
				(dt.sec == blinds_morning.sec)){
			motor_state = 1;
			direction = 0;
			currently_shaded = 0;
		} else if((dt.hour == blinds_night.hour) &
				(dt.min == blinds_night.min) &
				(dt.sec == blinds_night.sec)){
			motor_state = 1;
			direction = 1;
			currently_shaded = 1;
		}
	}
}

ISR(TIMER2_COMP_vect){ // set to 0,01s
	if(timer2_counter == 10){
		check_buttons = 1;		
		timer2_counter = 0;
	} else{
		timer2_counter++;
		check_buttons = 0; 
	}

	if(motor_state & (service_mode == 0)){
		if(shading_time == SHADING){
			motor_state = 0;
			shading_time = 0;
		} else{
			shading_time++;
		}
	} else if(motor_state & (service_mode == 1)){
		if(shading_time == (10 * service_shdtime)){
			motor_state = 0;
			shading_time = 0;
		} else{
			shading_time++;
		}
	}
}

void print_date(struct datetime dt){
	lcd_clear();
	lcd_set_xy(0,0);
	printf("Date: %02d/%02d/20%02d", dt.day, dt.month, dt.year);
	lcd_set_xy(1,0);
	printf("Time: %02d:%02d:%02d", dt.hour, dt.min, dt.sec);
}

void change_full_date(uint8_t currently_edited, char direction){
	// direction: (1) - add; (-1) - subtract
	if(direction == '+'){
		switch(currently_edited){
			case 0:
				if((current_date.month == 1) | (current_date.month == 3)
				| (current_date.month == 5) | (current_date.month == 7) 
				| (current_date.month == 8) | (current_date.month == 10) 
				| (current_date.month == 12)){
					current_date.day = current_date.day % 31 + 1;
				} else{
					if((current_date.month == 2) & (current_date.year % 4 == 0) & (current_date.year % 100 != 0)){
						current_date.day = current_date.day % 29 + 1;
					} else if(current_date.month == 2){
						current_date.day = current_date.day % 28 + 1;
					} else{
						current_date.day = current_date.day % 30 + 1;
					}	
				}						
				break;
			case 1:
				current_date.month = current_date.month % 12 + 1;
				if((current_date.month == 2) & (current_date.day >= 28)){
					current_date.day = 28;
				}
				break;
			case 2:
				current_date.year++;
				break;
			case 3:
				current_date.hour = (current_date.hour + 1) % 24;
				break;
			case 4: 
				current_date.min = (current_date.min + 1) % 60;
				break;
			case 5:
				current_date.sec = (current_date.sec + 1) % 60;
				break;
		}
	} else if(direction == '-'){
		switch(currently_edited){
			case 0:
				if((current_date.month == 1) | (current_date.month == 3)
				| (current_date.month == 5) | (current_date.month == 7) 
				| (current_date.month == 8) | (current_date.month == 10) 
				| (current_date.month == 12)){
					current_date.day = (current_date.day > 1) ? current_date.day - 1 : 1;
				} else{
					if((current_date.month == 2) & (current_date.year % 4 == 0) & (current_date.year % 100 != 0)){
						current_date.day = (current_date.day > 1) ? current_date.day - 1 : 1;
					} else if(current_date.month == 2){
						current_date.day = (current_date.day > 1) ? current_date.day - 1 : 1;
					} else{
						current_date.day = (current_date.day > 1) ? current_date.day - 1 : 1;
					}	
				}						
				break;
			case 1:
				current_date.month = (current_date.month > 1) ? current_date.month - 1 : 1;
				if((current_date.month == 2) & (current_date.day >= 28)){
					current_date.day = 28;
				}
				break;
			case 2:
				current_date.year = (current_date.year > 0) ? current_date.year - 1 : 0;
				break;
			case 3:
				current_date.hour = (current_date.hour > 0) ? current_date.hour - 1 : 0;
				break;
			case 4: 
				current_date.min = (current_date.min > 0) ? current_date.min - 1 : 0;
				break;
			case 5:
				current_date.sec = (current_date.sec > 0 ) ? current_date.sec - 1 : 0;
				break;
		}
	}
}

void change_blinds_harmonogram(struct datetime *edited_dt, uint8_t currently_edited, char direction){
	if(direction == '+'){
		switch(currently_edited){
			case 0:
				edited_dt->hour = (edited_dt->hour + 1) % 24;
				break;
			case 1: 
				edited_dt->min = (edited_dt->min + 1) % 60;
				break;
			case 2:
				edited_dt->sec = (edited_dt->sec + 1) % 60;
				break;
		}
	} else if(direction == '-'){
		switch(currently_edited){
			case 0:
				edited_dt->hour = (edited_dt->hour > 0) ? edited_dt->hour - 1 : 0;
				break;
			case 1: 
				edited_dt->min = (edited_dt->min > 0) ? edited_dt->min - 1 : 0;
				break;
			case 2:
				edited_dt->sec = (edited_dt->sec > 0 ) ? edited_dt->sec - 1 : 0;
				break;
		}
	}
}

void print_blinds_harmonogram(struct datetime *printed_dt, char time[]){
	lcd_clear();
	lcd_set_xy(0,0);
	printf("%s blinds", time);
	lcd_set_xy(1,0);
	printf("Time: %02d:%02d:%02d", printed_dt->hour, printed_dt->min, printed_dt->sec);
}


int main(void){
	// 0 - input, 1 - output
	DDRB = 0b00000001;	// switches and LED 
	PORTB = 0xff;
	DDRC = 0b11111110;	// blinds switch
	PORTC = 0xff;
	DDRD |= 0xf0;		// motor and CLK
	PORTD |= 0xf0;

	//Timer\Counter2 - 0,01s
	TCCR2 |= (1<<WGM01); // mode CTC
	TCCR2 |= (1<<CS22)|(1<<CS21)|(1<<CS20); //prescaler 1024
	OCR2 = 155; 	
	TIMSK |= (1<<OCIE2);

	lcdinit();
	I2C_init();

	_delay_ms(1000);

	MCUCR |= (1<<ISC01); 	// falling slope
	GICR |= (1<<INT0);		// turn INT0 interrupt on
	rtc_init();

	rtc_set_date_time(&current_date);
	
	//Initialize RS232
	USART_init(UBRR_VALUE);
	USART_send("--- BLINDS AUTOMATION ---\r\n");

	sei();

	while(1){
		if(check_buttons){	// check buttons in interval 0,1s
			if(!(PINB & (1<<SWITCH_EDIT))){
				// Enter time editing mode
				TURN_ON_LED;
				if(blinds_editing_mode != 1){
					update_data_from_ISR = 0;
					editing_mode = 1;
				}

			} else if(!(PINB & (1<<SWITCH_UP))){
				// Change currently edited (+)
				TURN_ON_LED;
				if(editing_mode){
					change_full_date(currently_edited, '+');
					print_date(current_date);
					_delay_ms(150);	
				} else if(blinds_editing_mode){
					switch(edited_blinds){
						case 0:
							change_blinds_harmonogram(&blinds_morning, currently_edited % 3, '+');
							print_blinds_harmonogram(&blinds_morning, "Morning");
							_delay_ms(150);
							break;
						case 1:
							change_blinds_harmonogram(&blinds_night, currently_edited % 3, '+');
							print_blinds_harmonogram(&blinds_night, "Night");
							_delay_ms(150);
							break;
					}
				}
			} else if(!(PINB & (1<<SWITCH_DOWN))){
				// Change currently edited (-)
				TURN_ON_LED;
				if(editing_mode){
					change_full_date(currently_edited, '-');
					print_date(current_date);
					_delay_ms(150);
				} else if(blinds_editing_mode){
					switch(edited_blinds){
						case 0:
							change_blinds_harmonogram(&blinds_morning, currently_edited % 3, '-');
							print_blinds_harmonogram(&blinds_morning, "Morning");
							_delay_ms(150);
							break;	
						case 1:
							change_blinds_harmonogram(&blinds_night, currently_edited % 3, '-');
							print_blinds_harmonogram(&blinds_night, "Night");
							_delay_ms(150);	
							break;
					}
				}
			} else if(!(PINB & (1<<SWITCH_NEXT))){
				// Switch currently edited element of datetime struct
				TURN_ON_LED;
				currently_edited = (currently_edited + 1) % 6;
				_delay_ms(300);
			} else if(!(PINB & (1<<SWITCH_SET))){
				// Accept changes
				TURN_ON_LED;
				if(editing_mode == 1){rtc_set_date_time(&current_date);}
				currently_edited = 0;
				editing_mode = 0;
				blinds_editing_mode = 0;
				update_data_from_ISR = 1;
				_delay_ms(100);
			} else if(!(PINC & (1<<SWITCH_BLINDS))){
				// Change blinds routine
				TURN_ON_LED;
				update_data_from_ISR = 0;
				if(!editing_mode){
					blinds_editing_mode = 1;
					edited_blinds = (edited_blinds + 1) % 2;
					switch(edited_blinds){
						case 0: //morning routine
							print_blinds_harmonogram(&blinds_morning, "Morning");
							break;
						case 1: //night routine
							print_blinds_harmonogram(&blinds_night, "Night");
							break;
					}
					_delay_ms(300);
				}
			} else {
				TURN_OFF_LED;
			}
		}

		if(motor_state){
			PORTD = (1<<(step+4));
			if(direction) step++;
			else step--;
			step = step % 4;
			_delay_ms(3);
		} else {
			PORTD = 0x00;
		}

		if(end){
			USART_send("\r\n");
			if( strncmp((void *) bufforRead, "on", 2) == 0 ){
				service_mode = 1;
				USART_send("Service mode enabled. \r\nMotor on \r\n");
				motor_state = 1;
			}
			else if( strncmp((void *) bufforRead, "off", 3) == 0 ){
				service_mode = 1;
				USART_send("Motor off \r\n");
				motor_state = 0;
			}
			else if( strncmp((void *) bufforRead, "direction", 9) == 0 ){
				service_mode = 1;
				uint8_t tmp_direction = atoi((const char*)bufforRead+10);
				if( (tmp_direction != 0) & (tmp_direction != 1)){
					USART_send("Direction not specified.\r\n");
				} else{
					direction = tmp_direction;
					USART_send("Direction specified.\r\n");
				}	
			}
			else if( strncmp((void *) bufforRead, "service-off", 11) == 0 ){
				USART_send("Service mode off.\r\n");
				service_mode = 0;
			}
			else{
				USART_send("Wrong command.\r\n");
				buffor_clear = 1;
			}
			end = 0;
			idx = 0;
			memset((void *)bufforRead,0,MAXSIZE);
		}
	}
	return 0;
}
