#include <stdint.h>
#include <stdbool.h>

#include   "hrtimer.h"

#define TCR  (*((volatile unsigned *) 0x40004004))
#define TC   (*((volatile unsigned *) 0x40004008))
#define PR   (*((volatile unsigned *) 0x4000400C))
#define MCR  (*((volatile unsigned *) 0x40004014))
#define CTCR (*((volatile unsigned *) 0x40004070))

uint32_t HrTimerCount()
{
    return TC;
}
uint32_t HrTimerSinceRepetitive(uint32_t* pLastCount)
{
    uint32_t thisCount = TC;
    uint32_t period = thisCount - *pLastCount;    
    *pLastCount = thisCount;
    return period;
}
uint32_t HrTimerSince(uint32_t lastCount)
{
    return TC - lastCount; 
}
uint32_t HrTimerSinceMs(uint32_t lastCount)
{
    uint32_t count = TC - lastCount;
    count += HR_TIMER_COUNT_PER_MS / 2; //Add 0.5 ms so the result is rounded to nearest ms
    return count / HR_TIMER_COUNT_PER_MS;
}

bool HrTimerRepetitiveTick(uint32_t* pLastCount, uint32_t interval)
{
    if (TC - *pLastCount >= interval) //All unsigned wrap around arithmetic
    {
        *pLastCount += interval;
        return true;
    }
    return false;
}

uint32_t HrTimerProRata(uint32_t value, uint32_t part)
{
    uint64_t fraction;
    
    fraction = part;
    fraction <<= 32;
    fraction /= HR_TIMER_COUNT_PER_SECOND;
              
    return (value * fraction) >> 32;
}

void HrTimerInit()
{    
    TCR     =     2; // 21.6.2 Timer Control Register - Reset TC and PC.
    CTCR    =     0; // 21.6.3 Count Control Register - Timer mode
    PR      =     0; // 21.6.5 Prescale register      - Don't prescale 96MHz clock (divide by PR+1).
    MCR     =     0; // 21.6.8 Match Control Register - no interrupt or reset
    TCR     =     1; // 21.6.2 Timer Control Register - Enable TC and PC
}
