#include <avr/io.h>
#include <avr/interrupt.h>

extern volatile uint16_t currentMilliSeconds;
extern volatile uint32_t currentSeconds;

extern void Timer0_initialize(void);
extern void Timer0_start(void);
extern void Timer0_stop(void);
extern void Timer0_reset(void);
