#include "SPI.h"
#include "Delay.h"
#include "UART.h"

#define DI PB5
#define DO PB6
#define SCK  PB7
#define CS PD6
#define SLAVE_CS PD2
void SPI_initializeMaster(void) {
   PORTB |= (1 << SCK) | (1 << DO);	//Configure SCK/MOSI/CS as output 
   CS_HIGH();
   DDRB  |= (1 << SCK) | (1 << DO);
   DDRB  &= ~(1 << DI);   //MISO is input
   DDRD  |= (1 << PD6);
   USICR = (1 << USIWM0) | (1 << USICS1);   //spi mode | clock source is positive edge using USCK pin
}

uint8_t SPI_send(uint8_t data) {  //only use with master mode
   uint8_t i,j;
   uint8_t sreg = SREG;   //save status reg
   cli();
   USISR &= 0xf0; //clear the counter
   PORTB &= ~(1 << SCK); //clear the clock line
   USIDR = data;
   uint8_t foo = USICR | (1 << USITC);
   CS_LOW();
   delay(1);
   for (i = 0; i < 16; ++i) {
         USICR = foo;
   } 
   CS_HIGH();
   SREG = sreg;
   return USIDR;
}

/*How SPI slave should work:
   When the CS line goes low... interrupt  to enable SPI_slave
   
   when overflow happens, an overflow interrupt shall go off and send the data it recieved via uart
   
   When the CS line goes high... interrupt to disable the SPI_slave
*/
void SPI_initializeSlave(void) {
   DDRB  |=  (1 << DO);
   DDRB  &= ~((1 << SCK) | (1 << DI) );
   USICR = (1 << USIWM0) | (1 << USICS1); //spi mode | clock source is positive edge using USCK pin
   //configure the pin change interrupt on INT0 -> PD2
   MCUCR &= ~(1 << ISC01);
   MCUCR |= (1 << ISC00);
   GIMSK |= (1 << INT0);
   UART_initialize();
}

ISR (USI_OVERFLOW_vect) {
   USISR |= (1 << USIOIF); //clear the status flag
   UART_sendByte(USIDR);
   PORTD |= (1 << PD5);
}


ISR (INT0_vect) {
   if (PIND & (1 << SLAVE_CS)) {
      //CS = High, device is not selected
      USICR &= ~(1 << USIOIE); //if the pin is high
      DDRB &= ~( (1 << SCK) | (1 << DO) | (1 << DI) );  //go into high impedance
   }
   else {
      USICR |= (1 << USIOIE); //if the pin is low
      DDRB &= ~( (1 << SCK) | (1 << DI) );
      DDRB |= (1 << DO);
   }
}

