//PROJETO FINAL - TURMA A
//ALARME COM DESPERTADOR SONORO
//FELIPE BRENO MELO DE AZEVEDO - 19/0106263
//ADRIEL LUCAS MORAIS CARVALHO - 19/0133619
#include <libs/gpio.h>
#include <libs/I2C.h>
#include <libs/lcd.h>
#include <libs/keyboard.h>
#include <msp430.h> 
#include <stdint.h>

void acorda();
void defAlarme();
void defHora();
void defData();

uint16_t     freq;
uint16_t     horaAlarme  = 0;
uint16_t     minAlarme   = 0;
uint8_t      hora[9]     = {0x00, 0x00, 0x3A, 0x00, 0x00, 0x3A, 0x00, 0x00, 0x00};
uint8_t      data[9]     = {0x00, 0x00, 0x2F, 0x00, 0x00, 0x2F, 0x00, 0x00, 0x00};
uint8_t      alarme[9]   = {0x30, 0x30, 0x3A, 0x30, 0x30, 0x3A, 0x30, 0x30, 0x00};
uint8_t      estado      = 0;
uint8_t      x           = 0;


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    pinMode(2, 0, output);      // Configura o pino 2.0 como saida
    P2SEL |= BIT0;              // Pino do buzzer

	RTCCTL01 = RTCMODE | RTCTEVIE | RTCAIE | RTCRDYIE;  //Config do RTC

	i2cConfig();                //Config do I2C
	lcdInit();                  //Config do LCD
	setup();                    //Config do teclado

	while(1)
	{
	    lcdWriteByte(0x02, 0);

	    //Pega a dezena e unidade das horas, minutos e segundos
        hora[0] = RTCHOUR/10 + 0x30;
        hora[1] = RTCHOUR%10 + 0x30;
        hora[3] = RTCMIN/10 + 0x30;
        hora[4] = RTCMIN%10 + 0x30;
        hora[6] = RTCSEC/10 + 0x30;
        hora[7] = RTCSEC%10 + 0x30;

        //Pega a dezena e unidade dos dias, meses e ano
        data[0] = RTCDAY/10 + 0x30;
        data[1] = RTCDAY%10 + 0x30;
        data[3] = RTCMON/10 + 0x30;
        data[4] = RTCMON%10 + 0x30;
        data[6] = RTCYEAR/10 + 0x30;
        data[7] = RTCYEAR%10 + 0x30;
        data[8] = 0x00;

        lcdPrint(hora);             //Printa o vetor das horas no formato hh:mm:ss
        lcdWriteByte(0xC0, 0);      //Pula para a segunda linha
        lcdPrint(data);             //Printa o vetor da data no formato dd/mm/aa

        if(estado)                  //Verificamos o estado do alarme
            lcdPrint("    ON");
        else
            lcdPrint("   OFF");

        if(pressC())                // Caso C seja pressionado vamos definir a data
        {
            defData();
            RTCDAY   = (data[1] - 0x30) + ( (data[0] - 0x30) * 10 );
            RTCMON   = (data[4] - 0x30) + ( (data[3] - 0x30) * 10 );
            RTCYEAR  = (data[7] - 0x30) + ( (data[6] - 0x30) * 10 );
            lcdWriteByte(0x01, 0);
        }

        if(pressB())                // Caso B seja pressionado vamos definir a hora
        {
            defHora();
            RTCHOUR  = (hora[1] - 0x30) + ( (hora[0] - 0x30) * 10 );
            RTCMIN   = (hora[4] - 0x30) + ( (hora[3] - 0x30) * 10 );
            lcdWriteByte(0x01, 0);
        }

        if(pressA())                // Caso A seja pressionado vamos definir o alarme
        {
            defAlarme();
            horaAlarme  = (alarme[1] - 0x30) + ( (alarme[0] - 0x30) * 10 );
            minAlarme   = (alarme[4] - 0x30) + ( (alarme[3] - 0x30) * 10 );
            RTCAHOUR    = horaAlarme;
            RTCAMIN     = minAlarme;
            RTCAHOUR |= BIT7;
            RTCAMIN |= BIT7;
            estado = 1;
            lcdWriteByte(0x01, 0);
        }

        if(pressCerq())             // Caso # seja pressionado vamos desativar o alarme
        {
            if(estado)
                estado--;
            else
                estado++;
        }

        if(RTCCTL01 & RTCAIFG)      // Se flag for setada (chegamos no valor do alarme)
        {
            if(estado)              // Se o alarme foi ativado pelo usuario vamos chamar a funcao que emite o som
                acorda();

            else                    // Se o alarme nao foi ativado  pelo usuario vamos apenas baixar a flag
                RTCCTL01 &= ~RTCAIFG;
        }

        if(pressD())                // Se D for pressionado desligamos o alarme sonoro
        {
            RTCCTL01 &= ~RTCAIFG;   // Flag � baixada
            TA1CCTL1 = OUTMOD_5;    // Modo reset (buzzer vai receber 0 e nao vai emitir som)
        }
	}



}

void defAlarme()
{
    lcdWriteByte(0x01, 0);
    lcdWriteByte(0x02, 0);
    lcdPrint("Defina o Alarme:");

    while(x < 6)                   // percorre os 6 valores do vetor alarme (HH:MM:SS)
    {
        uint8_t tecla = teclado(); // Funcaoo teclado retorna a tecla pressionada
                                   // se nenhuma tecla numerica foi pressionada o retorno � 10
        if(tecla != 10)            // Se alguma tecla numerica foi pressionada
        {
            alarme[x] = tecla;     // Salvamos o valor da tecla no vetor

            if(x == 1 || x == 4)   // Pulando a posicao dos ":"
                x = x + 2;
            else
                x++;
        }

        lcdWriteByte(0x02, 0);
        lcdWriteByte(0xC0, 0);
        lcdPrint(alarme);          // Printando as horas
    }

    x = 0;

    while(1)                        // Fica travado verificando se o alarme foi confirmado
    {                               // se ele nao for confirmado e uma tecla numerica for pressionada
        if(pressA())                // voltamos para o inicio do vetor e armazenamos o valor na primeira pos
            break;

        uint8_t tecla = teclado();

        if(tecla != 10)
        {
            alarme[x] = tecla;
            x++;
            defAlarme();
        }
    }
}

void defHora()                     // Funcionamento igual ao defAlarme
{
    lcdWriteByte(0x01, 0);
    lcdWriteByte(0x02, 0);
    lcdPrint("Defina a Hora:");

    while(x < 6)
    {
        uint8_t tecla = teclado();

        if(tecla != 10)
        {
            hora[x] = tecla;

            if(x == 1 || x == 4)
                x = x + 2;
            else
                x++;
        }

        lcdWriteByte(0x02, 0);
        lcdWriteByte(0xC0, 0);
        lcdPrint(hora);
    }

    x = 0;

    while(1)
    {
        if(pressB())
            break;

        uint8_t tecla = teclado();

        if(tecla != 10)
        {
            hora[x] = tecla;
            x++;
            defHora();
        }
    }
}

void defData()                      // Funcionamento igual ao defAlarme
{
    lcdWriteByte(0x01, 0);
    lcdWriteByte(0x02, 0);
    lcdPrint("Defina a Data:");

    while(x < 8)
    {
        uint8_t tecla = teclado();

        if(tecla != 10)
        {
            data[x] = tecla;

            if(x == 1 || x == 4)
                x = x + 2;
            else
                x++;
        }

        lcdWriteByte(0x02, 0);
        lcdWriteByte(0xC0, 0);
        lcdPrint(data);
    }

    x = 0;

    while(1)
    {
        if(pressC())
            break;

        uint8_t tecla = teclado();

        if(tecla != 10)
        {
            data[x] = tecla;
            x++;
            defData();
        }
    }
}

void acorda()                           // Funcao que manda a PWM pro buzzer
{
    TA1CTL = TASSEL__SMCLK | MC__CONTINUOUS | TACLR;

    TA1CCR0 = 0x8000;

    TA1CCR1 = TA1CCR0 >> 1;

    TA1CCTL1 = OUTMOD_4;
}

