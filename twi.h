#ifndef TWI_H_
#define TWI_H__

#include <avr/io.h>

#define ACK 0x01    //acknowledge
#define NO_ACK 0x00 //no acknowledge

void I2C_init(void);
uint8_t I2C_start(void);
void I2C_stop(void);
uint8_t I2C_write(uint8_t byte);
uint8_t I2C_read(uint8_t *value, uint8_t ack);

#endif