#include <libs/gpio.h>
#include <stdint.h>
#include <msp430.h>


    volatile  uint8_t  *  PDIR[]  =  {  &P1DIR,  &P1DIR,  &P2DIR,  &P3DIR,  &P4DIR,  &P5DIR,  &P6DIR,  &P7DIR}; //ponteiros para pos da memorias
    volatile  uint8_t  *  PREN[]  =  {  &P1REN,  &P1REN,  &P2REN,  &P3REN,  &P4REN,  &P5REN,  &P6REN,  &P7REN};
    volatile  uint8_t  *  POUT[]  =  {  &P1OUT,  &P1OUT,  &P2OUT,  &P3OUT,  &P4OUT,  &P5OUT,  &P6OUT,  &P7OUT};
    volatile  uint8_t  *  PIN[]  =  {  &P1IN,  &P1IN,  &P2IN,  &P3IN,  &P4IN,  &P5IN,  &P6IN,  &P7IN};

void pinMode(uint8_t port, uint8_t bit, pinMode_t mode) //Vamos configurar o píno dependendo em como o usuário vai pedir
{

    //bit = [0,1,2,... 7]
    //  -->  0x01  (0000.0001)
    //  -->  0x02  (0000.0010)
    //  -->  0x04  (0000.1000)
    //  -->  ...
    //  -->  0x80  (1000.0001)

    uint8_t mask = (0x01 << bit);   //Receber o valor 1 deslocado pra esquerda bit vezes

    if(mode == input)
    {
       *(  PDIR[port]  )  &=  ~mask;

    }

    if(mode == output)
    {
       *(  PDIR[port]  )  |=  mask;

    }

    if(mode == inPullUp)
    {
        *(  PDIR[port]  )  &=  ~mask;
        *(  PREN[port]   ) |=   mask;
        *(  POUT[port]   ) |=   mask;

    }

    if(mode == inPullDown)
    {
        *(  PDIR[port]  )  &=  ~mask;
        *(  PREN[port]   ) |=   mask;
        *(  POUT[port]   ) &=  ~mask;
    }
}

uint8_t pinRead(uint8_t port, uint8_t bit)
{

    return (  *PIN[port]  &  BIT(bit)  );
}

void pinWrite(uint8_t port, uint8_t bit, uint8_t value)
{

    if(value)
        *(  POUT[port]  )  |=  BIT(bit);
    else
        *(  POUT[port]  )  &= ~BIT(bit);
}
void pinInit()
{
    pinMode(REDLED, output);
    pinMode(GREENLED, output);
}

void pinToggle( uint8_t port, uint8_t bit)
{
    *(POUT[port]) ^=  BIT(bit);
}
