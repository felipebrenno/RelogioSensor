#include <libs/gpio.h>
#include <libs/I2C.h>
#include <libs/lcd.h>
#include <libs/timer.h>
#include <msp430.h>
#include <stdint.h>


// Nibble = agrupamento de 4 bits

void lcdWriteNibble(uint8_t nibble, uint8_t isChar)
{

    //| BT | EN | RW |
    i2cWriteByte(LCD, nibble << 4 | BT | 0  | 0  | isChar);
    i2cWriteByte(LCD, nibble << 4 | BT | EN | 0  | isChar);
    i2cWriteByte(LCD, nibble << 4 | BT | 0  | 0  | isChar);


}

void lcdWriteByte(uint8_t byte, uint8_t isChar)
{
    lcdWriteNibble(byte >> 4, isChar);          //MSBnibble
    lcdWriteNibble(byte & 0x0F, isChar);        //LSBnibble

}

uint8_t line = 0x00;

void lcdPrint(uint8_t * str)
{
    while(* str)
    {
        if(*str == '\n')
        {
            line ^=BIT6;
            lcdWriteByte(BIT7 | line, INSTR);
        }
        else
        {
            lcdWriteByte( *str, CHAR);
        }
        str++;
    }

}

void lcdInit()
{
    // Entra em modo 8 bits
    lcdWriteNibble(0x3, 0);
    lcdWriteNibble(0x3, 0);
    lcdWriteNibble(0x3, 0);

    // Entra em modo 4-bits
    lcdWriteNibble(0x2, 0);

    // Liga o cursor piscando e liga o display
    lcdWriteByte(0x0C, 0);

    // Return home and clear display
    lcdWriteByte(0x02, 0);
    lcdWriteByte(0x01, 0);

}
