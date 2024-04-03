#include <stdlib.h>
#include <stdbool.h>

#include "../../log/log.h"
#include "../../lpc1768/tm/time64.h"
#include "../../lpc1768/rtc/rtc.h"
#include "clktime.h"
#include "clk.h"
#include "clkutc.h"

#define GPREG0 (*((volatile unsigned *) 0x40024044))

volatile int32_t slew = 0; //ns     - up to +/- 2.147s of slew
volatile int32_t ppb  = 0; //This gets set to the last recorded ppb in TickInit

int32_t ClkGovGetSlew() { return slew; }
int32_t ClkGovGetPpb()  { return ppb;  }

void    ClkGovSetSlew(int32_t value) { slew = value; }
void    ClkGovSetPpb (int32_t value) { ppb  = value; GPREG0 = ppb; }
void    ClkGovInit()
{
    if (RtcPowerLost()) GPREG0 = 0;
    ppb = GPREG0;
}

//Clock limits
int ClkGovFreqDivisor         = 10;
int ClkGovFreqChangeMaxPpb    = 1000;
int ClkGovFreqSyncedLimPpb    = 1000;
int ClkGovFreqSyncedHysPpb    =  100;
int ClkGovSlewDivisor         = 10;
int ClkGovSlewChangeMaxMs     = 10;
int ClkGovSlewSyncedLimNs     = 10000000;
int ClkGovSlewSyncedHysNs     =  1000000;
int ClkGovSlewOffsetMaxSecs   = 3;

bool ClkGovTrace = false;

bool ClkGovIsReceivingTime = false; //This is set from the external source of time
bool ClkGovTimeIsSynced = false;
bool ClkGovRateIsSynced = false;
bool ClkGovIsSynced() { return ClkGovRateIsSynced && ClkGovTimeIsSynced; }

static void setTimeIsSyncedFlag(clktime diff)
{
    clktime absDiff = llabs(diff);
    clktime limit      = ClkGovSlewSyncedLimNs;
    clktime hysterisis = ClkGovSlewSyncedHysNs;

    if (absDiff < limit - hysterisis)
    {
        if (!ClkGovTimeIsSynced) LogTimeF("CLK Time sync acquired\r\n");
        ClkGovTimeIsSynced = true;
    }
    if (absDiff > limit + hysterisis)
    {
        if (ClkGovTimeIsSynced) LogTimeF("CLK Time sync lost (difference = %+lld)\r\n", diff);
        ClkGovTimeIsSynced = false;
    }
}
static void setRateIsSyncedFlag(clktime diff)
{

    clktime absDiff = llabs(diff);
    clktime limit      = ClkGovFreqSyncedLimPpb;
    clktime hysteresis = ClkGovFreqSyncedHysPpb;

    if (absDiff < limit - hysteresis)
    {
        if (!ClkGovRateIsSynced) LogTimeF("CLK Rate sync acquired\r\n");
        ClkGovRateIsSynced = true;
    }
    if (absDiff > limit + hysteresis)
    {
        if ( ClkGovRateIsSynced) LogTimeF("CLK Rate sync lost\r\n");
        ClkGovRateIsSynced = false;
    }
}

static void setSlew(clktime diff)
{
    clktime toAdd = -diff / ClkGovSlewDivisor;
    int32_t slewMaxTicks = ClkGovSlewChangeMaxMs << CLK_TIME_ONE_MS_ISH_SHIFT;

    if (toAdd >  slewMaxTicks) toAdd =  slewMaxTicks;
    if (toAdd < -slewMaxTicks) toAdd = -slewMaxTicks;

    slew = toAdd;

    if (ClkGovTrace) LogTimeF("CLK setSlew diff %lld gives slew %lld gives TickSlew %ld\r\n", diff, toAdd, slew);
}
static void adjustPpb(clktime diff)
{
    clktime toAdd = diff / ClkGovFreqDivisor;
    int32_t maxAdd = ClkGovFreqChangeMaxPpb;

    if (toAdd >  maxAdd) toAdd =  maxAdd;
    if (toAdd < -maxAdd) toAdd = -maxAdd;

    ClkGovSetPpb(ppb - toAdd);

    if (ClkGovTrace) LogTimeF("CLK setPpb diff %lld gives toAdd %lld gives TickPpb %ld\r\n", diff, toAdd, ppb);
}

static clktime lastIntClock = -1; //-1 indicates invalid value. 0 is a valid value.
static clktime lastExtClock = -1;
static void reset(clktime thisExtClock)
{
    ClkTimeSet(thisExtClock);
    ClkGovSetPpb(0);
    lastIntClock = 0;
    lastExtClock = 0;
}

static void sync(clktime thisExtClock, bool thisExtClockIsComplete)
{

    if (!ClkTimeIsSet() && thisExtClockIsComplete) //Cold start - only ever true if the RTC was not set.
    {
        LogTimeF("CLK cold start of clock so resetting\r\n");
        reset(thisExtClock);
        return;
    }

    //Get the time at the time of the interrupt
    clktime thisIntClock;
    clktime thisAbsClock;
    ClkTimesGetFromSnapshot(&thisIntClock, &thisAbsClock);

    //Calulate the time error
    clktime absDiff = thisAbsClock - thisExtClock;
    if (llabs(absDiff) > ((clktime)1 << (CLK_TIME_ONE_SECOND_SHIFT - 1)))
    {
        LogTimeF("CLK offset %lld (this - ext) is greater than half a second\r\n", absDiff);
    }
    if (llabs(absDiff) > ((clktime)ClkGovSlewOffsetMaxSecs << CLK_TIME_ONE_SECOND_SHIFT))
    {
        LogTimeF("CLK offset is greater than %d seconds so resetting\r\n", ClkGovSlewOffsetMaxSecs);
        reset(thisExtClock);
        return;
    }
    setSlew(absDiff);
    setTimeIsSyncedFlag(absDiff);

    //Calculate the rate error
    if (lastExtClock > -1)
    {
        clktime extPeriod = thisExtClock - lastExtClock;

        clktime intPeriod = thisIntClock - lastIntClock;
        clktime periodDiff =   intPeriod -    extPeriod;

        clktime ppbDiff;
        if (extPeriod == CLK_TIME_ONE_SECOND) ppbDiff =  periodDiff; //This saves a 64bit shift and division for PPS
        else                                  ppbDiff = (periodDiff << CLK_TIME_ONE_SECOND_SHIFT) / extPeriod;

        adjustPpb(ppbDiff);
        setRateIsSyncedFlag(ppbDiff);
    }

    //Save last values
    lastIntClock = thisIntClock;
    lastExtClock = thisExtClock;
}
void ClkGovSyncPpsI()
{
    ClkTimeSaveSnapshot();
}
void ClkGovSyncPpsN(time64 t) //t is number of seconds utc
{
    clktime time;
    time = (clktime)t << CLK_TIME_ONE_SECOND_SHIFT;
    time = ClkUtcToTai(time);
    sync(time, true);
}
void ClkGovSyncPpsZ()
{
    clktime time;
    time   = ClkNowTai();
    time  += 1UL << (CLK_TIME_ONE_SECOND_SHIFT - 1); //Add half a second so as to round to nearest rather than round down
    time >>= CLK_TIME_ONE_SECOND_SHIFT;
    time <<= CLK_TIME_ONE_SECOND_SHIFT;
    sync(time, false);
}


void ClkGovSyncTime(clktime time)
{
    ClkTimeSaveSnapshot();
    sync(time, true);
}