#include <avr/io.h>
#include <avr/interrupt.h>

extern void SD_read(unsigned long sector, unsigned short offset, unsigned char * buffer, unsigned short len);
extern void SD_readSector(uint32_t sector);
extern char SD_initialize(void);
extern uint8_t SD_command(uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t read);

#define SD_seekAddress( address) fat16_seekOffset( address )

extern void SD_seekOffset(unsigned long offset);

extern void SD_seekSector(unsigned long sector); 

extern char SD_read(unsigned char bytes);

#define SEEK_CUR 0
#define SEEK_SET 1

extern void SD_seekOffset(unsigned long offset, uint8_t origin);

#define CARD_NOT_INIT -1
#define CARD_IN_IDLE -2
#define CARD_INIT 0
