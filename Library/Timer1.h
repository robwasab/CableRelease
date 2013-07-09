#include <avr/io.h>
#include <avr/interrupt.h>

#define TURN_ON_OCR1A 0
#define TURN_ON_OCR1B 1
#define TURN_ON_BOTH  2

extern void Timer1_initializeFastPwm(uint8_t WHICH_MODULE_TO_TURN_ON);
extern void Timer1_stop(void);
extern void Timer1_start(void);
extern void Timer1_setOCR1A(uint16_t value);
extern void Timer1_setOCR1B(uint16_t value);
extern uint16_t Timer1_getValue(void);
