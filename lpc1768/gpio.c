#include "register.h"

void (*GpioHook)(void) = 0;

void GpioHandler()
{
	if (GpioHook) GpioHook();
}
void GpioInit()
{
    PCONP |= 1 << 15; //GPIO
    ISER0  = 1 << 21; //6.5.1 bit1 == Interrupt set enable for EINT3. It MUST be enabled even for GPIO interrupts - I checked.
}
void GpioInterruptsEnable()
{
	ISER0 = 1 << 21;
}
void GpioInterruptsDisable()
{
	ICER0 = 1 << 21;
}
static unsigned bitband2Address(int address, int bit)
{
	return ALIAS2 + ((((address - BASE2) << 3) + bit) << 2);
}
void GpioParse(char* pin, int* port, int* bit) //Expect something like "p0.12" or "P1-2" not the cpu or board pin 
{
	if (!pin) return;
	if (!port) return;
	if (!bit) return;
	
	*port = 0;
	*bit = 0;
	
	//First character - 'p' or 'P'
	if (*pin != 'p' && *pin != 'P') return;
	
	//Second character - port
	pin++;
	if (*pin < '0' || *pin > '3') return;
	*port = *pin - '0';
	
	//Third character - delimiter between port and bit
	pin++;
	if (*pin != ' ' && *pin != '.' && *pin != '-' && *pin != ':' && *pin != '/' && *pin != '[' && *pin != '(') return;
	
	//Fourth character - first bit decimal
	pin++;
	if (*pin < '0' || *pin > '9') return;
	*bit = *pin - '0';
	
	//Fifth character - second bit decimal
	pin++;
	if (*pin < '0' || *pin > '9') return;
	*bit = *bit * 10 + (*pin - '0');
}
unsigned* GpioSetPtr(char* pin)
{
	if (!pin) return 0;
	
	int port = 0;
	int bit = 0;
	GpioParse(pin, &port, &bit);
	
	switch (port)
	{
		case 0: return (unsigned*)bitband2Address(FIO0SET_ADDR, bit);
		case 1: return (unsigned*)bitband2Address(FIO1SET_ADDR, bit);
		case 2: return (unsigned*)bitband2Address(FIO2SET_ADDR, bit);
		default: return 0;
	}
}
unsigned* GpioClrPtr(char* pin)
{
	if (!pin) return 0;
	
	int port = 0;
	int bit = 0;
	GpioParse(pin, &port, &bit);
	
	switch (port)
	{
		case 0: return (unsigned*)bitband2Address(FIO0CLR_ADDR, bit);
		case 1: return (unsigned*)bitband2Address(FIO1CLR_ADDR, bit);
		case 2: return (unsigned*)bitband2Address(FIO2CLR_ADDR, bit);
		default: return 0;
	}
}
unsigned* GpioDirPtr(char* pin)
{
	if (!pin) return 0;
	
	int port = 0;
	int bit = 0;
	GpioParse(pin, &port, &bit);
	
	switch (port)
	{
		case 0: return (unsigned*)bitband2Address(FIO0DIR_ADDR, bit);
		case 1: return (unsigned*)bitband2Address(FIO1DIR_ADDR, bit);
		case 2: return (unsigned*)bitband2Address(FIO2DIR_ADDR, bit);
		default: return 0;
	}
}
unsigned* GpioPinPtr(char* pin)
{
	if (!pin) return 0;
	
	int port = 0;
	int bit = 0;
	GpioParse(pin, &port, &bit);
	
	switch (port)
	{
		case 0: return (unsigned*)bitband2Address(FIO0PIN_ADDR, bit);
		case 1: return (unsigned*)bitband2Address(FIO1PIN_ADDR, bit);
		case 2: return (unsigned*)bitband2Address(FIO2PIN_ADDR, bit);
		default: return 0;
	}
}