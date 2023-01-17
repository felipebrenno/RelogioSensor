
#ifndef LIBS_I2C_H_
#define LIBS_I2C_H_


#include <libs/timer.h>
#include <msp430.h>
#include <stdint.h>

#define SDA 2,5
#define SCL 2,4

// I2C por bit-banging
// Camada física

void release(uint8_t port, uint8_t bit);
void pullLow(uint8_t port, uint8_t bit);

// Operações de 1 bit

void writeBit(uint8_t value);
uint8_t readBit();
void start();
void stop ();

// Operações de vários bits

uint8_t writeByte(uint8_t value);
uint8_t readByte(uint8_t nack);

// Operações de linha

uint8_t i2cWrite(uint8_t addr, uint8_t * data, uint8_t size);
uint8_t i2cRead(uint8_t addr, uint8_t * data, uint8_t size);
void i2cConfig();
uint8_t i2cWriteByte(uint8_t addr, uint8_t byte);
#endif
