
#include <libs/gpio.h>
#include <libs/I2C.h>
#include <libs/timer.h>
#include <msp430.h>
#include <stdint.h>


// I2C por bit-banging
// Camada física

void release(uint8_t port, uint8_t bit)
{
    pinMode(port, bit, inPullUp);
}

void pullLow(uint8_t port, uint8_t bit)
{
    pinWrite(port, bit, 0);
    pinMode (port, bit, output);
}

// Operações de 1 bit

void writeBit(uint8_t value)
{
    if(value)
        release(SDA);                       // Escrevo o valor em SDA
    else
        pullLow(SDA);

    pullLow(SCL);                           // Aguardo T/4
    wait(10, ms);

    release(SCL);                           // Faço bater o clock por 2T/4
    wait(20, ms);

    pullLow(SCL);                           // Aguardo T/4
    wait(10, ms);
}

uint8_t readBit()
{
    release(SDA);                           // Solto o SDA para o escravo escrever
    pullLow(SCL);                           // Faço o clock ir para zero
    wait(10, ms);                           // Aguardo T/4

    release(SCL);                           // Bato o Clock (dou T/4 para o bit ser certeiro)
    wait(10, ms);

    uint8_t bit = pinRead(SDA);             // Leio o bit enviado pelo escravo (SDA) na metade do pulso do Clock
    wait(10, ms);                           // Espero T/4

    pullLow(SCL);                           // Termino de bater o clock, descendo ele
    wait(10, ms);

    return bit;                             // Retorno o valor lido
}

void start()
{
    release(SDA);                           // Considero que SDA = 1
    release(SCL);                           // Considero que SCL = 1

    wait(10, ms);                           // Espero T/4 para gerar o
    pullLow(SDA);                           // flanco de descida de SDA

    wait(10, ms);                           // Depois preparo o clock
    pullLow(SCL);                           // para a transmissão dos bits

    wait(20, ms);                           // Garante que o start gasta T ms
}

void stop()
{
    pullLow(SDA);                           // Considero que SCL = 0
    wait(10, ms);                           // SDA é desconhecido, então fazemos SDA = 0

    release(SCL);                           // Faço SCL = 1, para gerar o flanco em SDA
    wait(10, ms);                           // Aguardo T/4

    release(SDA);                           // Gero o flanco de subida em SDA
    wait(20, ms);                           // caracteriza a sinalização de stop
}

// Operações de vários bits

uint8_t writeByte(uint8_t value)
{
    uint8_t mask = 0x80;                    // Começamos pelo MSB

    while(mask)
    {
        writeBit(value & mask);             // Filtrando o valor de "value"
        mask = mask >> 1;                   // com a máscara "mask"
    }

    return readBit();                       // Ciclo de ACK/NACK
}

uint8_t readByte(uint8_t nack)
{
    uint8_t mask = 0x80;                    // Começamos pelo MSB
    uint8_t value = 0x00;

    while(mask)                             // Recebemos todos os bits, um a um
    {                                       // setando o valor apontado pela máscara
        if( readBit() )                     // na variável value.
        {
            value |= mask;
        }

        mask = mask >> 1;                   // Deslocamos mask para avançar o pro pxrox bit
    }
    writeBit(nack);                         // Ciclo de ACK/NACK, o mestre que escreve na linha
    return value;
}

// Operações de linha

uint8_t i2cWrite(uint8_t addr,uint8_t * data, uint8_t nBytes )
{
    UCB1IFG = 0;                                    // Recomendado zerar a flag de IFG antes de começar
                                                    // a comunicação

    UCB1I2CSA = addr;                               // Endereço do escravo
    UCB1CTL1 |= UCTXSTT | UCTR;                     // Start e modo transmissor

    while(!(UCB1IFG & UCTXIFG));                    // Escreve no buffer de TX
    UCB1TXBUF = *data++;                           // para destravar o ciclo de ACK
    nBytes--;

    while(UCB1CTL1 & UCTXSTT );                     // Espera o ciclo de ACK/NACK acontecer

    if(UCB1IFG & UCNACKIFG)                         // Se o escravo não estiver presente na linha
    {                                               // vou receber um NACK
        UCB1CTL1 |= UCTXSTP;                        // Sou obrigado então a transmitir um STOP
        while(UCB1CTL1 & UCTXSTP);                  // Espero o stop ser de fato transmitido
        return 1;                                   // e retorno um código de erro
    }
    while(nBytes--)
    {
        while(!(UCB1IFG & UCTXIFG));                // Escreve no buffer de TX
        UCB1TXBUF = *data++;                       // para destravar o ciclo de ACK
    }
    while(!(UCB1IFG & UCTXIFG));                    // Espere o último byte ser carregado
                                                    // no registro de deslocamento

    UCB1CTL1 |= UCTXSTP;                        // Peço um STOP
    while(UCB1CTL1 & UCTXSTP);                  // Espero o stop ser de fato transmitido

    return 0;
}
uint8_t i2cRead(uint8_t addr, uint8_t * data, uint8_t size)
{
    start();
    uint8_t nack = writeByte(addr << 1 | 1);

    if(nack)
    {
        stop();
        return nack;
    }
    while(size--)
    {
        *data++ = readByte( !size );
    }
    stop();

    return nack;
}

uint8_t i2cWriteByte(uint8_t addr, uint8_t byte)
{
    return i2cWrite(addr, &byte, 1);
}

void i2cConfig()
{
    UCB1CTL1 = UCSWRST;                             // Reseta a interface
    UCB1CTL0 = UCMST  |  UCMODE_3  | UCSYNC;        // Interface é mestre, modo I2C síncrono
    UCB1CTL1 |= UCSSEL__SMCLK;                      // Vamos usar smclk @1mhz
    UCB1BRW  = 100;                                 // Usando divisor SCL @10KHZ = SMCLK/100

    P4SEL |= BIT1 | BIT2;                           // P3.0 e P3.1 serão usados pela interface dedicada
    P4DIR &= ~(BIT1 | BIT2);                        // DIR = 0 para usar os resistores
    P4OUT |= BIT1 | BIT2;
    P4REN |= BIT1 | BIT2;                           // Não eh bom usar pull up na propria placa e sim por fora no circuito

    UCB1CTL1 &= ~UCSWRST;                           // Zera o bit de RST para deixar a interface funcionar
}

