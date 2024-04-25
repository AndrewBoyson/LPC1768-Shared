#include <stdint.h>

#include "lpc1768/register.h"

void PwmInit(uint32_t freq, uint32_t max)
{
    PCONP    |= 1 <<  6;  //PWM1
    PCLKSEL0 |= 1 << 12;  //PWM1
    PINSEL4  |= 1U << 0;  //P2.00 01 PWM1.1 --> p26
	
    uint32_t pre = 96000000 / max / freq; //eg max = 100, freq = 400Hz ==> pre == 2400
	
    PWM1TCR     =        2; // 24.6.2 PWM Control Register   - Count reset
    PWM1PCR     =        0; // 24.6.6 PWM Control Register   - PWM1 output disabled
    PWM1CTCR    =        0; // 24.6.3 Count Control Register - Timer mode
    PWM1PR      =  pre - 1; // 21.6.5 Prescale register      - Prescale 96MHz clock (divide by PR+1).
    PWM1MCR     =        2; // 24.6.4 Match Control Register    - resets on MR0
    PWM1MR0     =      max; // Set the width
    PWM1MR1     =        0; // Leave the output off
    PWM1LER     =        3; // 24.6.7 PWM Latch Enable Register - transfer MR0 and MR1 values after next reset
    PWM1PCR     =   1 << 9; // 24.6.6 PWM Control Register      - PWM1 output enabled
    PWM1TCR     =        9; // 24.6.2 Timer Control Register    - Enable PWM and reset and start timer
}

void PwmSet(uint32_t value)
{
    if (value > PWM1MR0) value = PWM1MR0;
    PWM1MR1 = value;
    PWM1LER = 2;          // 24.6.7 PWM Latch Enable Register - transfer MR1 value after next reset
}