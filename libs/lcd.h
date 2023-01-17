

#ifndef LIBS_LCD_H_
#define LIBS_LCD_H_

#define CHAR    1
#define INSTR   0
#define LCD 0x27  //0x27 ou 0x3F
#define BT BIT3
#define EN BIT2
#define RW BIT1
#define RS BIT0


void lcdPrint(uint8_t * str);
void lcdInit();
void lcdWriteByte(uint8_t byte, uint8_t isChar);


#endif
