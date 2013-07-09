#include "Print.h"

void printString(char * s)
{
	while(*s != '\0')
	{
		GENERIC_OUTPUT(*s);
		++s;
	}
}


void printNibble(uint8_t b)
{
	b &= 0x0f;
	if (b < 0x0a)
	{
		GENERIC_OUTPUT(0x30 | b);
	}
	else
	{
		b -= 9;
		GENERIC_OUTPUT(0x40 | b);
	}
}

void printInt(uint8_t * b, uint8_t bytes)
{
    bytes -= 1;
	printString("0x");
	do {
	   printNibble(b[bytes] >> 4);
	   printNibble(b[bytes]);
	}
	while (bytes--);
}

