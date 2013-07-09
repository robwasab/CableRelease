#include "Timer0.h"

/*Configure Timer 1 ISR to fire every 1ms, This can be used for a System time, and in 10ms I need to call an Timer process function in diskio.c.*/

volatile uint8_t prescale = 0;
void Timer0_stop() {
   prescale = TCCR0B & 0x07;
   TCCR0B &= 0xf8;
}

void Timer0_start() {
   TCCR0B |= prescale;
}

void Timer0_reset() {
   TCNT0 = 0x00;
}

void Timer0_initialize(void) {
   TCCR0A = (1 << WGM01);   //CTC mode
   //TCCR0B gets configured in configRegistersForThisPeriod(period)
   /*setting OCR0A to 181 and prescale of 1 configures the timer to interrupt 44.1kHz which is the audio play back frequency of CD's*/
   /* counding 441 of the interrupts = 10 ms of time*/
   OCR0A = 181;  
   TCCR0B = (1 << CS00); 
   TIMSK |= (1 << OCIE0A);   
}

volatile uint16_t currentMilliSeconds = 0;
volatile uint32_t currentSeconds = 0;
volatile uint16_t interruptCounter = 0;

ISR (TIMER0_COMPA_vect) {  
   ++interruptCounter;
   /*Play back code in here that executes 44.1kHz 

   */ 
   if (interruptCounter >= 221) {
      interruptCounter = 0;
      currentMilliSeconds += 10;
   }
   if (currentMilliSeconds >= 1000) {
      currentMilliSeconds = 0;
      ++currentSeconds;
   }
}
