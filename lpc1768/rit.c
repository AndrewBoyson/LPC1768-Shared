
#include "lpc1768/led.h"

#define RICOMPVAL (*((volatile unsigned *) 0x400B0000)) //Compare register R/W 0xFFFF FFFF
#define RIMASK    (*((volatile unsigned *) 0x400B0004)) //Mask    register R/W 0
#define RICTRL    (*((volatile unsigned *) 0x400B0008)) //Control register R/W 0xC
#define RICOUNTER (*((volatile unsigned *) 0x400B000C)) //32-bit  counter  R/W 0
#define ISER0     (*((volatile unsigned *) 0xE000E100))

void (*RitHook)() = 0;

void __attribute__((interrupt ("IRQ")))RitHandler()
{
    RICTRL = 0xF; //Clear interrupt flag (1); clear on compare (2); debug (4); timer enable (8) 
    if (RitHook) RitHook();
}
void RitInit(int ms)
{
    if (ms)
    {
        RICOMPVAL = 24000 * ms; //One ms if PCLK at 96MHz / 4.
        RICTRL    =        0xF; //Clear interrupt flag (1); clear on compare (2); debug (4); timer enable (8) 
        ISER0    |=   1U << 29; //6.5.1 bit29 == Interrupt set enable for RIT.
    }
}
