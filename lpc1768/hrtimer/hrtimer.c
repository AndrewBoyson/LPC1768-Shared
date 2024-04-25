#include <stdint.h>
#include <stdbool.h>

#include "lpc1768/register.h"
#include "hrtimer.h"

uint32_t HrTimerCount()
{
    return T0TC;
}
uint32_t HrTimerSinceRepetitive(uint32_t* pLastCount)
{
    uint32_t thisCount = T0TC;
    uint32_t period = thisCount - *pLastCount;    
    *pLastCount = thisCount;
    return period;
}
uint32_t HrTimerSince(uint32_t lastCount)
{
    return T0TC - lastCount; 
}
uint32_t HrTimerSinceMs(uint32_t lastCount)
{
    uint32_t count = T0TC - lastCount;
    count += HR_TIMER_COUNT_PER_MS / 2; //Add 0.5 ms so the result is rounded to nearest ms
    return count / HR_TIMER_COUNT_PER_MS;
}

bool HrTimerRepetitiveTick(uint32_t* pLastCount, uint32_t interval)
{
    if (T0TC - *pLastCount >= interval) //All unsigned wrap around arithmetic
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
    PCONP    |= 1U <<  1; //TIMER0
    PCLKSEL0 |= 1U <<  2; //TIM0
	
    T0TCR     =     2; // 21.6.2 Timer Control Register - Reset TC and PC.
    T0CTCR    =     0; // 21.6.3 Count Control Register - Timer mode
    T0PR      =     0; // 21.6.5 Prescale register      - Don't prescale 96MHz clock (divide by PR+1).
    T0MCR     =     0; // 21.6.8 Match Control Register - no interrupt or reset
    T0TCR     =     1; // 21.6.2 Timer Control Register - Enable TC and PC
}
