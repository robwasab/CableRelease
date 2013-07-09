#include "Timer1.h"
#include "Servo.h"

void Servo_init()
{
   Timer1_initializeFastPwm(TURN_ON_OCR1A);
   //Override Settings
   //Set top to 0x09C4
   ICR1 = 0x09C4;
   //Set Divider to 64
   TCCR1B |= (1 << CS11);
   //start at the Max position, specific to Xander's project
   OCR1A = SERVO_MIN;
   DDRB |= (1 << PB3);
}

void Servo_reset()
{
   OCR1A = SERVO_MAX;
}

void Servo_inc()
{
   if (OCR1A < SERVO_MAX) OCR1A += 10;
} 
   
void Servo_dec()
{
   if (OCR1A > SERVO_MIN) OCR1A -= 10;
} 

void Servo_setPos(uint16_t val) 
{
   if (val > SERVO_MAX) 
   {
      OCR1A = SERVO_MAX;
      return;
   }
   if (val < SERVO_MIN)
   {
      OCR1A = SERVO_MIN;
      return;
   }
   OCR1A = val;
}
