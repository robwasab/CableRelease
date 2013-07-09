#include <avr/io.h>
#include <avr/interrupt.h>

#define MAX_STRING_SIZE 50
void UART_sendChar(char foo);
void UART_sendByte(uint8_t foo);
void UART_initialize(void);
void UART_sendString(char* string);
uint8_t UART_getByte();

#define UART_getChar() (char)UART_getByte()

