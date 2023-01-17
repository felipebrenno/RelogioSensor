#include <stdint.h>

typedef enum {input, output, inPullUp, inPullDown} pinMode_t; //Declarando uma enumeração de literais

#define BIT(n) (1 << n)

#define REDLED 1,0
#define GREENLED 4,7
#define SW1    2 ,1
#define SW2    1,1

void pinMode(uint8_t port, uint8_t bit, pinMode_t mode);
uint8_t pinRead(uint8_t port, uint8_t bit);
void pinWrite(uint8_t port, uint8_t bit, uint8_t value);
void pinInit();
void pinToggle( uint8_t port, uint8_t bit);
