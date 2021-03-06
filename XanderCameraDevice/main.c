#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "../Library/UART.h"
#include "../Library/Delay.h"
#include "../Library/Timer1.h"
#include "../Library/Timer0.h"
#include "../Library/LCD.h"
#include "../Library/Print.h"
#include "../Library/Servo.h"
#include <avr/pgmspace.h> 
#include <avr/eeprom.h>
#include "util/delay.h"

typedef struct
{
   uint16_t timer;
   uint16_t holdTime;
   //uint8_t led;
} Settings;

#define TRUE 1
#define FALSE 0

//UP has PCINT14
#define UP PD3
//DOWN has PCINT13
#define DOWN PD2
//SELECT has PCINT15
#define SELECT PD4

#define LED_PIN PD5
#define SERVO PB3

uint8_t LED = LED_PIN;

#define PCIE2 4

volatile uint8_t cursor = 0, select = 0;
uint16_t timer, holdTime = 1;

void pollUpDownButtons(uint16_t * val) 
{
   cli();
   do 
   {   
      lcd_goto(0,0);
      printInt(val, sizeof(uint16_t));
      //Polling is needed here
      if (! (PIND & (1 << UP)) ) 
      {
         _delay_ms(100);
         if (! (PIND & (1 << UP)) ) 
         {
            ++*val;
         }
      }
      if (! (PIND & (1 << DOWN)) ) 
      {
         _delay_ms(100);
         if (! (PIND & (1 << DOWN)) ) 
         {
            --*val;
         }
      }
   } while(PIND & (1 << SELECT));
   
   //wait until SELECT is RELEASED
   while(! (PIND & (1 << SELECT)) );
   _delay_ms(500);
   sei();
}

/*
LCD View Stuff
Pointer Madness
*/
typedef struct {
   char name[17];
   void (*action)(void);
}MenuItem;

typedef struct {
   uint8_t len;
   MenuItem menuItems[];
}Menu;


/*SET TIMER Object*/
void setTimer() 
{
   pollUpDownButtons(&timer);
   //save timer
   eeprom_write_word((uint16_t *)0, timer);
   eeprom_busy_wait();
}

/*SET HOLD TIME OBJECT*/
void setHoldTime()
{
   pollUpDownButtons(&holdTime);
   eeprom_write_word((uint16_t *)2, holdTime);
   eeprom_busy_wait();   
}

/*TEST OBJECT*/
void test()
{
   lcd_goto(0,0);
   lcd_puts("SEL TO EXIT");
   lcd_goto(0,1);
   lcd_puts("DOWN TO TRIG");
   cli();
   while(PIND & (1 << SELECT)) 
   {
      if (!(PIND & (1 << DOWN)))
      {
         OCR1A = SERVO_MAX;
         PORTD |= (1 << LED);
      }
      else 
      {
         OCR1A = SERVO_MIN;
         PORTD &= ~(1 << LED);
      }
   }
   _delay_ms(500);
   OCR1A = SERVO_MIN;
   sei();
}    

/*START OBJECT*/
void start()
{
   lcd_goto(0,0);
   lcd_puts("PRESS SEL TO XIT");
   currentSeconds = 0;
   while(timer > currentSeconds) 
   {
      lcd_goto(0,1);
      printInt(&currentSeconds, sizeof(uint16_t));
      lcd_puts(" ~ ");
      printInt(&timer, sizeof(uint16_t));      
      if (select)
      {
         select = FALSE;
         return;
      }
   }
   
   PORTD |= (1 << LED);
   OCR1A = SERVO_MAX;
   currentSeconds = 0;
   while (holdTime > currentSeconds);
   OCR1A = SERVO_MIN;
   PORTD &= ~(1 << LED);
}

/*RESET Object*/
void reset()
{
   LED = LED_PIN;
   timer = 0;
   holdTime = 0;
   eeprom_write_word((uint16_t *)0, timer);
   eeprom_busy_wait();
   eeprom_write_word((uint16_t *)2, holdTime);
   eeprom_busy_wait();
   sei();
}

/*LED Object*/
void toggleLed()
{
   LED ^= LED_PIN;
   if (LED) lcd_puts("ON");
   else lcd_puts("OFF");
   _delay_ms(1000);
}

const Menu menu PROGMEM = 
{6, 
   { 
   {">SET TIMER      ", &setTimer},
   {">SET HOLD TIME  ", &setHoldTime},
   {">START          ", &start},
   {">TEST           ", &test},
   {">TOGGLE LED     ", &toggleLed},
   {">RESET          ", &reset}
   }
};

uint8_t buffer[sizeof(menu)];

void fillBuffer(Menu* obj)
{
   //get the lengh, always the first byte
   buffer[0] = pgm_read_byte(obj);
   uint8_t i = 1, j, k = buffer[0];
   do {
      for (j = 0; j < sizeof(MenuItem); ++j) 
      {
         buffer[i] = pgm_read_byte(obj + i);
         ++i;
      }
   } while(--k);
}

void displayBuffer(uint8_t* buf)
{
   
   lcd_goto(0,0);
   lcd_puts( ((Menu*) buf)->menuItems[cursor].name);

   lcd_goto(0,1);
   //if your not the cursor then I will shift the string to the left to get rid of '>'
   if (cursor <  (buffer[0] - 1 ) )
   lcd_puts( (((Menu*) buf)->menuItems[cursor+1].name) + 1 );

}

/*End LCD pointer madness*/

//interrup.h avr website isn't helpful with finding interrupt vectors
//went straight to the include file: /usr/local/CrossPack-AVR/avr/include/avr
//There is a PCINT for each PORT it matches up perfectly 

ISR (PCINT_D_vect) {
   _delay_ms(50);
   uint8_t button = ~PIND;
   button &= ( (1 << UP) | (1 << DOWN) | (1 << SELECT) );
   
   //A double check, if a button wasn't pushed down then GTFO mate
   if (!button) return;
   
   switch (button)
   {
      case (1 << DOWN):
         if (cursor < (((Menu*)buffer)->len)-1) ++cursor;

         break;
      case (1 << UP):
         if (cursor > 0) --cursor;
         
         break;
      case (1 << SELECT):
         select = TRUE;
         break;
    }
        
    //wait until the buttons are released
    do {
       //clear out all pins but the buttons
       button = PIND & ( (1 << UP) | (1 << DOWN) | (1 << SELECT) );
       //wait until ALL of the buttons are HIGH
       if (button == ( (1 << UP) | (1 << DOWN) | (1 << SELECT) ) ) 
          break;
    }
    while (1);
}

int main(void) 
{
   //initialization
   
   //PD6 is for the LCD Screen
   DDRD = (1 << PD6) | (1 << LED_PIN);
   //For the LCD SCREEN
   DDRB = 0xff;
   //Enable pull-up resistors on UP DOWN and SELECT pins
   PORTD |= (1 << UP) | (1 << DOWN) | (1 << SELECT);

   //Turn off un needed modules
   PRR = (1 << PRUSI) | (1 << PRUSART);
   

   //Start the Servo Early 
   Servo_init();
   
   //Enable Pin Change Interrupts on PCINT 13 -> 15
   GIMSK |= (1 << PCIE2);
   PCMSK2 = (1 << PCINT13) | (1 << PCINT14) | (1 << PCINT15);
   
   timer = eeprom_read_word((uint16_t *)0);
   eeprom_busy_wait();
   holdTime = eeprom_read_word((uint16_t *)2);
   
   lcd_init();
   lcd_clear();
   lcd_goto(0,0);
   lcd_puts("Designed By:");
   lcd_goto(0,1);
   lcd_puts("Robby Tong");
   _delay_ms(2000);

   Timer0_initialize();
   
   sei();
   
   while (1) 
   {

   fillBuffer(&menu);

   do {
      lcd_clear();
      displayBuffer(buffer);
      _delay_ms(100);
   } while(!select);
   select = FALSE;
   lcd_clear();
   //call the menu item action!!! OBJECT ORIENTED...
   (*((Menu *)buffer)->menuItems[cursor].action)();
   }
   return 0;
}
