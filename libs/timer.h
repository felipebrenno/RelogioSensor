
#ifndef LIBS_TIMER_H_
#define LIBS_TIMER_H_

#define MAX_TIMERS 10

#include <libs/timer.h>
#include <msp430.h>
#include <stdint.h>

typedef enum {us, ms, sec, min} timeunit_t;


uint32_t timerNew(uint32_t time, timeunit_t unit );
uint16_t timerIsRunning(uint16_t t);
void wait(uint16_t time, timeunit_t unit );






#endif /* LIBS_TIMER_H_ */
