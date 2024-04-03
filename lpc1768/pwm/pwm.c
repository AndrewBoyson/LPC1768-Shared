#include <stdint.h>

#include "log/log.h"

#define TCR  (*((volatile unsigned *) 0x40018004))
#define TC   (*((volatile unsigned *) 0x40018008))
#define PR   (*((volatile unsigned *) 0x4001800C))
#define MCR  (*((volatile unsigned *) 0x40018014))
#define MR0  (*((volatile unsigned *) 0x40018018))
#define MR1  (*((volatile unsigned *) 0x4001801C))
#define PCR  (*((volatile unsigned *) 0x4001804C))
#define LER  (*((volatile unsigned *) 0x40018050))
#define CTCR (*((volatile unsigned *) 0x40018070))

void PwmInit(uint32_t freq, uint32_t max)
{
    uint32_t pre = 96000000 / max / freq; //eg max = 100, freq = 400Hz ==> pre == 2400
    TCR     =        2; // 24.6.2 PWM Control Register   - Count reset
    PCR     =        0; // 24.6.6 PWM Control Register   - PWM1 output disabled
    CTCR    =        0; // 24.6.3 Count Control Register - Timer mode
    PR      =  pre - 1; // 21.6.5 Prescale register      - Prescale 96MHz clock (divide by PR+1).
    MCR     =        2; // 24.6.4 Match Control Register    - resets on MR0
    MR0     =      max; // Set the width
    MR1     =        0; // Leave the output off
    LER     =        3; // 24.6.7 PWM Latch Enable Register - transfer MR0 and MR1 values after next reset
    PCR     =   1 << 9; // 24.6.6 PWM Control Register      - PWM1 output enabled
    TCR     =        9; // 24.6.2 Timer Control Register    - Enable PWM and reset and start timer
}

void PwmSet(uint32_t value)
{
    if (value > MR0) value = MR0;
    MR1 = value;
    LER = 2;          // 24.6.7 PWM Latch Enable Register - transfer MR1 value after next reset
}