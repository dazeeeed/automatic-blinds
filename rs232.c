#include "rs232.h"

volatile char *bufor;
volatile uint8_t sign = 0;
volatile uint8_t bufforRead[MAXSIZE];
volatile uint8_t idx= 0;
volatile uint8_t end = 0;

void USART_init(uint8_t ubrr){
    //Speed selection
    UBRRH = (uint8_t) (ubrr>>8);
    UBRRL = (uint8_t) ubrr;

    // Turn receiving and transmission mode
    UCSRB |= (1<<RXEN) | (1<<TXEN);

    // Turn receiving interrupt on 
    UCSRB |= (1<<RXCIE);

    // Data format: 8bit, 0 bit parity, 1 bit stop
    UCSRC |= (1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1);
}

void USART_wait_for_empty(void){
    while( !(UCSRA & (1<<UDRE)) ); 
    // UDRE=1 when buffor is ready for getting data
}

void USART_send(char *text){
    USART_wait_for_empty();
    bufor = (uint8_t *) text;
    UCSRB |= (1<<UDRIE); // Turn transmission interrupt on
}

ISR(USART_RXC_vect){
    sign = UDR;
    if(idx>=(MAXSIZE-1)) idx = 0;
    bufforRead[idx] = sign;
    idx++;
    if(sign =='\n' || sign == '\r') end = 1;
    else end = 0;
}

ISR(USART_UDRE_vect){
    if(*bufor) UDR = *(bufor++);
    else UCSRB &= ~(1<<UDRIE);
}
