#include <avr/io.h>
#include <avr/interrupt.h>
#include "UART.h"
#include "Delay.h"
#include "Timer1.h"
#include "Timer0.h"
#include "SPI.h"
#include "SD.h"

#define TX_PIN PD1
#define OCR1A_PIN PB3
#define OCR1B_PIN PB4

void main() {				//start main

   DDRD = (1 << TX_PIN);
   DDRB = (1 << PB0) | (1 << OCR1A_PIN) | (1 << OCR1B_PIN);
   UART_initialize();
   Timer1_initializeFastPwm(TURN_ON_BOTH);
   Timer0_initialize();
   SPI_initialize();
   UART_sendByte(SD_initialize());  //0 good, -1 not good, -2 not good
   sei();
   uint16_t pwmVal = 1;
   uint8_t i = 0;

   uint8_t buf[32] = {0};
   SD_read(5, 0x55, buf, 32);
   for (i = 0; i<32; ++i) {
      UART_sendByte(buf[i]);
   }
   while(1) {		//start while 1  
      /*
      Timer1_setOCR1A(pwmVal);
      Timer1_setOCR1B(pwmVal);
      pwmVal = (pwmVal == 0) ? 1 : pwmVal << 1;
      */
      /*
      switch(UART_getByte()) {
         case 1:
            SD_command(0x40, 0x00000000, 0x95, 8);
            break;
         case 2:
            SD_command(0x41, 0x00000000, 0xFF, 8);
            break;
         case 3:
            SD_command(0x50, 0x00000200, 0xFF, 8);
            break;
         case 4:
            SD_readSector(0);
            break;
         }
       */
   }					//end while 1
}					//end main

