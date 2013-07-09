#include <avr/io.h>
#include <avr/interrupt.h>

#define SS_LOW PB4

void delay(unsigned long int x)
{
	for (unsigned long int j=0; j<x; j++)
	{
		for (unsigned int i=0; i<100; i++) //137 more=better
		{
			asm("nop");
		}
	}
}

#define  enableUSI() USICR |= (1 << USIWM0)
#define disableUSI() USICR &= ~( (1 << USIWM1) | (1 << USIWM0) )
#define serialSend(x)   while( !(UCSRA & (1<<UDRE))); UDR = x

void UART_sendChar(char foo) {
   while ( !(UCSRA & (1 << UDRE)) );
   UDR = foo;
   while ( !(UCSRA & (1 << TXC)) );
}

void UART_sendByte(uint8_t foo) {
   while ( !(UCSRA & (1 << UDRE) ) );
   UDR = foo;
   while ( !(UCSRA & (1 << TXC)) );
}


void initialize() {
//=================-USART-===========================
   UCSRA = (1 << U2X);
   UCSRB = (1 << TXEN);   //self explainatory
   UCSRC = (1 << UCSZ1) | (1 << UCSZ0);  //8 bit character size
   UBRRH = 0x01;   
   UBRRL = 0xA0;  // (baud rate 9600) w/ clock @ 8MHZ   
//=================-SPI-=============================
   //mode0
   //DDRB = 0; nothing to be done, usually set DO as output but I'm not using DO at the moment
  // USICR =  (1 << USICS1); //enable USI timer overflow interrupt | take samples on positive edge
//=================-PIN CHANGE INTERRUPT-============
}



int main() {				//start main
DDRD = (1 << PD1);
initialize();
   char i = 0;
   while(1) {		//start while 1  
      UART_sendByte(i++);
      delay(10000);
   }					//end while 1
}					//end main

