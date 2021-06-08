#ifndef RS232_H
#define RS232_H

#include<avr/io.h>
#include<avr/interrupt.h>

#define MAXSIZE 32 //max buffor (32)

extern volatile char *bufor;
extern volatile uint8_t sign;
extern volatile uint8_t bufforRead[MAXSIZE];
extern volatile uint8_t idx;
extern volatile uint8_t end;

void USART_init(uint8_t ubrr);
void USART_send(char *text);
void USART_wait_for_empty(void);

#endif