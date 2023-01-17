#ifndef LIBS_KEYBOARD_H_
#define LIBS_KEYBOARD_H_

#define LINE1 2,5
#define LINE2 2,4
#define LINE3 1,5
#define LINE4 1,4

#define COL1 1,3
#define COL2 1,2
#define COL3 4,3
#define COL4 4,0

#include <msp430.h>
#include <stdint.h>


void setup();
void turnOFF();
uint8_t teclado();
uint8_t verifica();
uint8_t pressA();
uint8_t pressB();
uint8_t pressC();
uint8_t pressD();
uint8_t pressCerq();




#endif /* LIBS_KEYBOARD_H_ */
