#include "rs232.h"

volatile char *bufor;
volatile uint8_t znak = 0;
volatile uint8_t bufforRead[MAXSIZE];
volatile uint8_t indeks= 0;
volatile uint8_t end = 0;

void USART_init(uint8_t ubrr){
    //Wybor predkosci
    UBRRH = (uint8_t) (ubrr>>8);
    UBRRL = (uint8_t) ubrr;

    //Wlaczenie mozliwosci odczytu jak i nadawania
    UCSRB |= (1<<RXEN) | (1<<TXEN);

    //Wlaczenie przerwania do odczytu
    UCSRB |= (1<<RXCIE);

    //Format ramki: 8bit, 0 bit parzystosci, 1 bit stop
    UCSRC |= (1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1);
}

void USART_wait_for_empty(void){
    while( !(UCSRA & (1<<UDRE)) ); 
    //UDRE=1 gdy budor jest gotowy na otrzymanie danych
}

void USART_send(char *text){
    USART_wait_for_empty();
    bufor = (uint8_t *) text;
    UCSRB |= (1<<UDRIE); //wlaczenie przerwania do transmisji
}

ISR(USART_RXC_vect){
    znak = UDR;
    if(indeks>=(MAXSIZE-1)) indeks = 0;
    bufforRead[indeks] = znak;
    indeks++;
    if(znak =='\n' || znak == '\r') end = 1;
    else end = 0;
}

ISR(USART_UDRE_vect){
    if(*bufor) UDR = *(bufor++);
    else UCSRB &= ~(1<<UDRIE);
}

/*
#include "rs232.h"

volatile char *bufor;
volatile uint8_t znak=0;

void USART_init(uint8_t ubrr)
{
//Wybór prędkości
UBRRH=(uint8_t)(ubrr>>8);
UBRRL=(uint8_t)ubrr;

//Włączenie możliwości odczytu jak i nadawania
UCSRB|=(1<<RXEN)|(1<<TXEN);

//Wlaczenie przerwania do odczytu
UCSRB|=(1<<RXCIE);

//Format ramki: 8bit, 0 bit parzystości, 1bit stopu
UCSRC|=(1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
}



void USART_wait_for_empty(void)
{
while(!(UCSRA & (1<<UDRE))); //UDRE=1 gdy bufor UDR jest gotowy na otrzymanie danych
}



void USART_send(char *text)
{
USART_wait_for_empty();
bufor=(uint8_t *)text;
UCSRB|=(1<<UDRIE); //Wlaczenie przerwania do transmisji
}



ISR(USART_RXC_vect)
{
znak=UDR;
}



ISR(USART_UDRE_vect)
{
if(*bufor) UDR=*(bufor++);
else UCSRB&=~(1<<UDRIE);
}
*/