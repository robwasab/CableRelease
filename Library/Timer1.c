#include "Timer1.h"

/*Notes
Accessing the low byte triggers the 16-bit read or write operation. When the low byte of a 16-bit register is written by the CPU, the high byte stored in the temporary register, and the low byte written are both copied into the 16-bit register in the same clock cycle. When the low byte of a 16-bit register is read by the CPU, the high byte of the 16-bit register is copied into the tempo- rary register in the same clock cycle as the low byte is read.
Not all 16-bit accesses uses the temporary register for the high byte. Reading the OCR1A/B 16- bit registers does not involve using the temporary register.

To do a 16-bit write, the high byte must be written before the low byte. For a 16-bit read, the low byte must be read before the high byte.

This operation is taken care of in C.
Assembly you have to 
mov r16, TCNT1L ;now the temp is stored with TCNT1H
mov r17, TCNT1H

is the equivalent in C: int i = TCNT1
*/

/*These functions configure Timer1 for fast pwm. the two functions configure OC1A and OC1B outputs respectively*/

void Timer1_initializeFastPwm(uint8_t WHICH_MODULE_TO_TURN_ON) {
   TCCR1A = (1 << WGM11) ; //WGM configures the waveform
   TCCR1B =  (1 << WGM12) | (1 << WGM13) | (1 << CS10);  //no prescalling, the clock is the F_CPU clock
   ICR1 = 0xffff;
   switch (WHICH_MODULE_TO_TURN_ON) {
      case TURN_ON_OCR1A:
         TCCR1A |= (1 << COM1A1);
         break;
      case TURN_ON_OCR1B:
         TCCR1A |= (1 << COM1B1);
         break;
      case TURN_ON_BOTH:
         TCCR1A |= (1 << COM1A1) | (1 << COM1B1);
   }
}

volatile uint8_t savePrescale = 0;

void Timer1_stop() {
   savePrescale = (0x07 & TCCR1B);   //save the prescale bits
   TCCR1B &= 0xf8;   //clear the prescale to stop the timer
}

void Timer1_start() {
   TCCR1B |= savePrescale;
}

uint16_t Timer1_getValue() {
/*
   uint8_t sreg = SREG;   //save status reg
   cli();
   uint16_t value = TCNT1L;   //temp register get updated with TCNT1H after this executes
            value |= TCNT1H << 8;
   SREG = sreg;
*/
   return   TCNT1;
}

void Timer1_setOCR1A(uint16_t value) {
/*
   uint8_t sreg = SREG;   //save status reg
   cli();
   OCR1AH = value >> 8;
   OCR1AL = (uint8_t) value; 
   SREG = sreg;
*/
   OCR1A = value;
}

void Timer1_setOCR1B(uint16_t value) {
/*
   uint8_t sreg = SREG;   //save status reg
   cli();
   OCR1BH = value >> 8;
   OCR1BL = (uint8_t) value; 
   SREG = sreg;
*/
   OCR1B = value;
}

