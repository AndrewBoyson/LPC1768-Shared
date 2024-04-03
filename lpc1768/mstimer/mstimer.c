#include <stdint.h>
#include <stdbool.h>

#include "mstimer.h"
#include "lpc1768/hrtimer/hrtimer.h"

uint32_t MsTimerCount = 0;

bool MsTimerAbsolute(uint32_t untilMs)                            //This uses signed comparison so it is limited to 24 days
{
    return (int)(MsTimerCount - untilMs) > 0;
}

bool MsTimerRelative(uint32_t baseMsCount, uint32_t intervalMs) //This uses unsigned comparison so it is limited to 49 days
{
    return MsTimerCount - baseMsCount >= intervalMs;
}

bool MsTimerRepetitive(uint32_t* pBaseMsCount, uint32_t intervalMs)
{
    if (MsTimerCount - *pBaseMsCount >= intervalMs) //All unsigned wrap around arithmetic
    {
        *pBaseMsCount += intervalMs;
        return true;
    }
    return false;
}

void MsTimerMain()
{
    static uint32_t baseCount = 0;
    
    if (HrTimerRepetitiveTick(&baseCount, HR_TIMER_COUNT_PER_MS)) MsTimerCount++;
}