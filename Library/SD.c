#include "SD.h"
#include "SPI.h"
#include "UART.h"
#include "Timer0.h"

/*Init and go to SPI mode: ]r:10 [0x40 0x00 0x00 0x00 0x00 0x95 r:8]
Initialize card: [0x41 0x00 0x00 0x00 0x00 0xFF r:8]
Set transfer size: [0x50 0x00 0x00 0x02 0x00 0xFF r:8]
Read sector: [0x51 0x00 0x00 0x00 0x00 0xFF r:520]
*/

#define CARD_READY 0
#define CARD_BUSY 1

uint8_t SD_command(uint8_t cmd, uint32_t arg, 
                uint8_t crc, uint8_t read) {
        
    CS_LOW();
    SPI_send(cmd);
    SPI_send(arg>>24);
    SPI_send(arg>>16);
    SPI_send(arg>>8);
    SPI_send(arg);
    SPI_send(crc);
    uint8_t i;      
    for(i=0; i<read; i++) {
         if (0x01 == SPI_send(0xFF) ) { //card sent back a 1 == ready 
            CS_HIGH();
            return CARD_READY;
         }
    }
    CS_HIGH();    
    return CARD_BUSY;           
}


unsigned long sd_sector;
unsigned short sd_pos;

char SD_initialize(void) {
    char i;
    cli();
    // ]r:10
    CS_HIGH();
    for(i=0; i<10; i++) // idle for 1 bytes / 80 clocks
        SPI_send(0xFF);
                
    // [0x40 0x00 0x00 0x00 0x00 0x95 r:8] until we get "1"
    for(i=0; i<10 && SD_command(0x40, 0x00000000, 0x95, 8) != 1; i++) {
       delay_ms(100);
    }
    if(i == 10) // card did not respond to initialization
        return CARD_NOT_INIT;
                
    // CMD1 until card comes out of idle, but maximum of 10 times
    for(i=0; i<10 && SD_command(0x41, 0x00000000, 0xFF, 8) != 0; i++) {
       delay_ms(100);
    }

    if(i == 10) // card did not come out of idle
        return CARD_IN_IDLE;
                
    // SET_BLOCKLEN to 512
    SD_command(0x50, 0x00000200, 0xFF, 8);
        
    sd_sector = sd_pos = 0;
        
    return CARD_INIT;
}

// TODO: This function will not exit gracefully if SD card does not do what it should
void SD_read(unsigned long sector, unsigned short offset, unsigned char * buffer, 
             unsigned short len) {
    unsigned short i, pos = 0;
    
    CS_LOW();
    SPI_send(0x51);       // sector * 512 = sector << 9 
    SPI_send(sector>>15); // sector*512 >> 24  
    SPI_send(sector>>7);  // sector*512 >> 16
    SPI_send(sector<<1);  // sector*512 >> 8
    SPI_send(0);          // sector*512
    SPI_send(0xFF);
    
    for(i=0; i<10 && SPI_send(0xFF) != 0x00; i++) {} // wait for 0
    
    for(i=0; i<10 && SPI_send(0xFF) != 0xFE; i++) {} // wait for data start
    
    for(i=0; i<offset; i++) // "skip" bytes
        SPI_send(0xFF);
        
    for(i=0; i<len; i++) // read len bytes
        buffer[i] = SPI_send(0xFF);
        
    for(i+=offset; i<512; i++) // "skip" again
        SPI_send(0xFF);
        
    // skip checksum
    SPI_send(0xFF);
    SPI_send(0xFF);    

    CS_HIGH();    
}

char SD_buffer[32];

#define fat16_seekAddress( address) fat16_seekOffset( address )

void SD_seekOffset(unsigned long offset, uint8_t origin) {
    switch (origin) {
       case SEEK_CUR:
          sd_sector += offset >> 9;
          sd_pos += offset & 511;
          break;
       case SEEK_SET:
          sd_sector = offset >> 9;
          sd_pos = offset & 511;   
    }  
}

void SD_seekSector(unsigned long sector) {
   sd_sector = sector;
   sd_pos = 0;
}

char SD_read(unsigned char bytes) {
    SD_read(sd_sector, sd_pos, fat16_buffer, bytes);
    sd_pos+=(unsigned short)bytes;
    
    if(sd_pos == 512) {
        sd_pos = 0;
        sd_sector++;
    }
    
    return bytes;
}
