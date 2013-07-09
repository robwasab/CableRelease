void
lcd_clear(void);
void
lcd_puts(const char * s);
void
lcd_putch(unsigned char c);
void
lcd_goto(unsigned char pos,unsigned char line);
void
lcd_init(void);

//#define DEBUG

#ifdef DEBUG
   void test(void);
#endif
