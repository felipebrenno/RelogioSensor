#include <libs/timer.h>
#include <msp430.h>
#include <stdint.h>

uint16_t count;


void wait(uint16_t time, timeunit_t unit )
{
    if(unit == us)
    {

        TA2CTL = TASSEL__SMCLK | MC__UP | TACLR;
        TA2CCR0 = time;

        while(!(TA2CCTL0 & CCIFG));                         // Trava de interrupção
        TA2CCTL0 &= ~CCIFG;                                 // Resetando a flag

    }
    else
    {
        TA2CTL = TASSEL__ACLK | MC__UP | TACLR;
        if(unit == ms)
        {

            TA2CCR0 = (time << 5) - 1;                     // N = time * 32758 / 1024 = time * 2^5
                                                           // Aproximando 1000 por 1024
            while(!(TA2CCTL0 & CCIFG));
            TA2CCTL0 &= ~CCIFG;

        }
        if(unit == sec)
        {

            count = time;
            TA2CCR0 = 0x8000 -1;
            TA2CCTL0 = CCIE;

            while(count);
        }
        if(unit == min)
        {

            count = time * 60;
            TA2CCR0 = 0x8000 -1;
            TA2CCTL0 = CCIE;

            while(count);

        }
    }
    TA2CTL = MC__STOP;
}

#pragma vector = TIMER2_A0_VECTOR
__interrupt void TA2_CCR0_ISR()
{
    count--;
}
