#include <stdint.h>
#include <stdio.h>

#include "../../log/log.h"
#include "../../lpc1768/rtc/rtc.h"
#include "../../lpc1768/hrtimer/hrtimer.h"
#include "../../lpc1768/led.h"
#include  "clktime.h"
#include   "clkgov.h"
#include    "clktm.h"
#include   "clkutc.h"

#define ONE_BILLION 1000000000

static clktime timeThisScan = 0;
static clktime timeAtStart  = 0;

clktime ClkNowTai() { return timeThisScan; }
clktime ClkRefTai() { return timeAtStart; }

void    ClkNowTmLocal(struct tm* ptm) { ClkTimeToTmLocal(timeThisScan, ptm); }
void    ClkNowTmUtc  (struct tm* ptm) { ClkTimeToTmUtc  (timeThisScan, ptm); }

void ClkNowAscii(char* p)
{
    struct tm tm;
    ClkTimeToTmUtc(timeThisScan, &tm);
    sprintf(p, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}
void ClkInit()
{
    ClkGovInit();
    ClkUtcInit();
}

void ClkMain()
{
    static uint32_t secondsTimer = 0;
    
    bool hadSecond = HrTimerRepetitiveTick(&secondsTimer, HR_TIMER_COUNT_PER_SECOND);
    
    //Update the times whenever there has been a system second
    if (hadSecond) ClkTimeIncrementByOneSecond(secondsTimer);

    //Record the time this scan to save everyone doing a ClkTimeGet
    timeThisScan = ClkTimeGet();

    //Update the leap seconds count if required
    ClkUtcCheckAdjustLeapSecondCount(timeThisScan);

    //Record the time the clock started
    if (ClkGovIsSynced()) timeAtStart = timeThisScan;

    //Keep the clock and the RTC synchronised
    if (ClkTimeIsSet())
    {
        //Save the time to the RTC on the second
        static bool wasSecond = false;
        bool isSecond = timeThisScan & (1 << CLK_TIME_ONE_SECOND_SHIFT);
        if (wasSecond && !isSecond)
        {
            struct tm tm;
            ClkTimeToTmUtc(timeThisScan, &tm);
            RtcSetTm(&tm);
        }
        wasSecond = isSecond;
    }
    else
    {
        //Start the clock from the RTC if not started
        if (RtcIsSet())
        {
            static int lastRtcSecond = -1;
            struct tm tm;
            RtcGetTm(&tm);
            if (lastRtcSecond > 0 && tm.tm_sec != lastRtcSecond)
            {
                timeThisScan = ClkTimeFromTmUtc(&tm);
                ClkTimeSet(timeThisScan);
                LogTimeF("Clock set from RTC\r\n");
            }
            lastRtcSecond = tm.tm_sec;
        }
    }
}