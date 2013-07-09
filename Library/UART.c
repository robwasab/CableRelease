#include "UART.h"
#include "Delay.h"

void UART_sendString(char* string) {
   int i;
   for (i = 0; i < MAX_STRING_SIZE && string[i] != '\0'; ++i) UART_sendChar(string[i]);
}

void UART_sendChar(char foo) {
   while ( !(UCSRA & (1 << UDRE)) );
   UDR = foo;
   while ( !(UCSRA & (1 << TXC)) );
   delay(100);
}

void UART_sendByte(uint8_t foo) {
   while ( !(UCSRA & (1 << UDRE) ) );
   UDR = foo;
   while ( !(UCSRA & (1 << TXC)) );
   delay(100);
}

uint8_t UART_getByte() {
   while ( !(UCSRA & (1 << RXC)) ); //wait for a byte to come in 
   return UDR;
}


void UART_initialize() {
//=================-USART-===========================
   UCSRA = (1 << U2X);
   UCSRB = (1 << TXEN) | (1 << RXEN);   //self explainatory
   UCSRC = (1 << UCSZ1) | (1 << UCSZ0);  //8 bit character size
   UBRRH = 0x01;   
   UBRRL = 0xA0;  // (baud rate 2400) w/ clock @ 8MHZ   
//=================-SPI-=============================
   //mode0
   //DDRB = 0; nothing to be done, usually set DO as output but I'm not using DO at the moment
  // USICR =  (1 << USICS1); //enable USI timer overflow interrupt | take samples on positive edge
//=================-PIN CHANGE INTERRUPT-============
}

