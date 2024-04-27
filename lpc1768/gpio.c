#include "register.h"

void (*GpioHook)(void) = 0;

void GpioHandler()
{
	if (GpioHook) GpioHook();
}

void GpioInit()
{
    PCONP |= 1 << 15; //GPIO
    ISER0 |= 1 << 21; //6.5.1 bit1 == Interrupt set enable for EINT3. It MUST be enabled even for GPIO interrupts - I checked.
}