#include "twi.h"

void I2C_init(void){
    TWBR = 72;  //72 for 16MHz gives 100kHz TWI SCLK
    TWSR &= ~((1<<TWPS0)|(1<<TWPS1));   // No prescaler
}

uint8_t I2C_start(void){
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    while(!(TWCR & (1<<TWINT)));
    return (TWSR & 0xf8);
}

void I2C_stop(void){
    TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
    while(!(TWCR & (1<<TWSTO)));
}

uint8_t I2C_write(uint8_t byte){
    TWDR = byte;
    TWCR = (1<<TWINT) | (1<<TWEN);
    while(!(TWCR & (1<<TWINT)));
    return (TWSR & 0xf8);
}

uint8_t I2C_read(uint8_t *value, uint8_t ack){
    TWCR = (1<<TWINT) | (1<<TWEN) | (ack<<TWEA);
    while(!(TWCR & (1<<TWINT)));
    *value = TWDR;
    return (TWSR & 0xf8);
}