#include <libs/gpio.h>
#include <libs/keyboard.h>
#include <libs/timer.h>
#include <msp430.h>
#include <stdint.h>

uint8_t i = 0;
uint8_t k = 0;

void turnOFF()                                                  // Desliga todos os pinos
{
    P2OUT &= ~BIT5;                                             // Linha 1
    P2OUT &= ~BIT4;                                             // Linha 2
    P1OUT &= ~BIT5;                                             // Linha 3
    P1OUT &= ~BIT4;                                             // Linha 4
}

void setup()                                                    // Configura os pinos da linhas e colunas
{                                                               // sendo as linhas saidas
    pinMode(LINE1,output);                                      // e as colunas entradas com resistor de pullDown
    pinMode(LINE2,output);
    pinMode(LINE3,output);
    pinMode(LINE4,output);

    pinMode(COL1,inPullDown);
    pinMode(COL2,inPullDown);
    pinMode(COL3,inPullDown);
    pinMode(COL4,inPullDown);

}

uint8_t pressA()                                                // Funcao que verifica se a tecla A foi pressionado
{
    turnOFF();                                                  // desliga todos os pinos
    P2OUT |=  BIT5;                                             // liga a linha 1

    if(pinRead(COL4))                                           // Se a linha 1 e coluna 4 foi pressionada o A foi pressionado
    {
        turnOFF();                                              // desligamos os pinos
        wait(300,ms);
        return 1;                                               // retornamos 1 confirmando que foi pressionado
    }

    turnOFF();
    return 0;
}

uint8_t pressB()                                                // Mesma coisa da pressA, mas com B
{
    turnOFF();
    P2OUT |=  BIT4;

    if(pinRead(COL4))
    {
        turnOFF();
        wait(300,ms);
        return 1;
    }

    turnOFF();
    return 0;
}

uint8_t pressC()    //...
{
    turnOFF();
    P1OUT |=  BIT5;

    if(pinRead(COL4))
    {
        turnOFF();
        wait(300,ms);
        return 1;
    }

    turnOFF();
    return 0;
}

uint8_t pressD()    //...
{
    turnOFF();
    P1OUT |=  BIT4;

    if(pinRead(COL4))
    {
        turnOFF();
        wait(300,ms);
        return 1;
    }

    turnOFF();
    return 0;
}

uint8_t pressCerq() //...
{
    turnOFF();
    P1OUT |=  BIT4;

    if(pinRead(COL3))
    {
        turnOFF();
        wait(300,ms);
        return 1;
    }

    turnOFF();
    return 0;
}

uint8_t verifica()                                              // Funcao que verifica quais teclas numericas foram pressionadas e retorna seu valor
{
    if(pinRead(COL1))
    {
        switch(i)
        {
        case 1:
            k = 1;
            return 1;
        case 2:
            k = 1;
            return 4;
        case 3:
            k = 1;
            return 7;
        }
    }
    if(pinRead(COL2))
    {
        switch(i)
        {
        case 1:
            k++;
            return 2;
        case 2:
            k++;
            return 5;
        case 3:
            k++;
            return 8;
        case 4:
            k++;
            return 0;
        }
    }
    if(pinRead(COL3))
    {
        switch(i)
        {
        case 1:
            k++;
            return 3;
        case 2:
            k++;
            return 6;
        case 3:
            k++;
            return 9;
        }
    }

    return 10;
}

uint8_t teclado()                                               // Funcao de funcionamento do teclado
{
    uint16_t valor = 0;

    i = 0;

    turnOFF();                                                  // Desligamos os pinos
    P2OUT |=  BIT5;                                             // Ligamos a linha 1
    i++;                                                        // Indicamos que estamos na linha 1
    valor = verifica();                                         // Verificamos as teclas
    if(k)                                                       // Se alguma tecla foi pressionada
    {                                                           // retornamos seu valor ja pronto para o LCD
        k = 0;
        wait(300,ms);
        return valor + 0x30;
    }


    turnOFF();
    P2OUT |=  BIT4;
    i++;
    valor = verifica();
    if(k)
    {
        k = 0;
        wait(300,ms);
        return valor + 0x30;
    }


    turnOFF();
    P1OUT |= BIT5;
    i++;
    valor = verifica();
    if(k)
    {
        k = 0;
        wait(300,ms);
        return valor + 0x30;
    }

    turnOFF();
    P1OUT |= BIT4;
    i++;
    valor = verifica();
    if(k)
    {
        k = 0;
        wait(300,ms);
        return valor + 0x30;
    }

        return 10;
}

