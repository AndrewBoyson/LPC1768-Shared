#include "register.h"

void (*MsRitHook)() = 0;

void __attribute__((interrupt ("IRQ")))MsRitHandler()
{
    RICTRL = 0xF; //Clear interrupt flag (1); clear on compare (2); debug (4); timer enable (8) 
    if (MsRitHook) MsRitHook();
}
void MsRitInit()
{
    PCONP    |= 1U << 16; //RIT
	RICOMPVAL =    24000; //One ms if PCLK at 96MHz / 4.
	RICTRL    =      0xF; //Clear interrupt flag (1); clear on compare (2); debug (4); timer enable (8) 
	ISER0     = 1U << 29; //6.5.1 bit29 == Interrupt set enable for RIT.
}
