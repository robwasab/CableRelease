#include <avr/io.h>
#include <avr/interrupt.h>

#define CS_LOW()	PORTD &= ~(1 << PD6)                    /* CS=low */
#define	CS_HIGH()	PORTD |=  (1 << PD6)			/* CS=high */

#define SPI_SEND_FAST 0
#define SPI_SEND_SLOW 1

extern void SPI_initializeMaster(void);
extern void SPI_initializeSlave(void);  //this function will initialize the UART 
extern uint8_t SPI_send(uint8_t data);
