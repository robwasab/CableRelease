#include "UART.h"
#include "LCD.h"

void printString(char * s);
void printInt(uint8_t * b, uint8_t bytes);

//Redefine GENERIC_OUTPUT(byte) to put a byte on the LCD Screen!
#define GENERIC_OUTPUT(byte) lcd_putch(byte);

//#define GENERIC_OUTPUT(foo) UART_sendByte(foo)

