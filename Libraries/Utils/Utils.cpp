#include <lightuino5.h>
#include "Utils.h"

//?? Printf-style to the USB

void print(unsigned long int num,char base)
{
	Usb.print(num,base);
}

void print(const char* str)
{
	Usb.print(str);
}

void println(const char* str)
{
	Usb.println(str);
}

void println(unsigned long int num,char base)
{
	Usb.println(num,base);
}