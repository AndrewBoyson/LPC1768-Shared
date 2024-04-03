#include <stdint.h>
#include <stdbool.h>

#include "../../lpc1768/rtc/rtc.h"
#include "../../lpc1768/tm/tm.h"
#include "../../lpc1768/hrtimer/hrtimer.h"
#include "../../lpc1768/interrupts.h"
#include "../../lpc1768/led.h"
#include "../../log/log.h"
#include "clk.h"
#include "clktime.h"
#include "clkgov.h"

static clktime tickCount = 0;
static clktime slewCount = 0;
static bool   countIsSet = false;

bool    ClkTimeIsSet() { return countIsSet; }

static uint32_t hrTimerAtLastIncrement = 0; //Set by the increment function

clktime ClkTimeGet()
{
    return tickCount + slewCount + HrTimerProRata(CLK_TIME_ONE_SECOND + ClkGovGetPpb() + ClkGovGetSlew(), HrTimerSince(hrTimerAtLastIncrement));
}

void ClkTimeSet(clktime extClock)
{    
     clktime timerCountSinceLastSecond = HrTimerSince(hrTimerAtLastIncrement);
     clktime fraction = (timerCountSinceLastSecond << CLK_TIME_ONE_SECOND_SHIFT) / HR_TIMER_COUNT_PER_SECOND;
     clktime    ticks = extClock - fraction;

    InterruptsDisable();
        tickCount = ticks;
        slewCount = 0;
    InterruptsEnable();
    
    countIsSet = true;
}
void ClkTimeAdjustSeconds(int seconds)
{
    InterruptsDisable();
        tickCount += (clktime)seconds << CLK_TIME_ONE_SECOND_SHIFT;
    InterruptsEnable();
}
void ClkTimeIncrementByOneSecond(uint32_t startCount)
{
    InterruptsDisable();
        hrTimerAtLastIncrement = startCount;
        tickCount += CLK_TIME_ONE_SECOND + ClkGovGetPpb();
        slewCount += ClkGovGetSlew();
        ClkGovSetSlew(0);
    InterruptsEnable();
}

static volatile  clktime  tickSnapshot;
static volatile  clktime  slewSnapshot;
static volatile uint32_t timerSnapshot;

void ClkTimeSaveSnapshot()
{
     timerSnapshot = HrTimerSince(hrTimerAtLastIncrement);
      tickSnapshot = tickCount;
      slewSnapshot = slewCount;
}
void ClkTimesGetFromSnapshot(clktime* pInt, clktime* pAbs)
{
    *pInt = tickSnapshot                + HrTimerProRata(CLK_TIME_ONE_SECOND + ClkGovGetPpb(),                   timerSnapshot);
    *pAbs = tickSnapshot + slewSnapshot + HrTimerProRata(CLK_TIME_ONE_SECOND + ClkGovGetPpb() + ClkGovGetSlew(), timerSnapshot);
}