#include <stdint.h>

#include "clktime.h"
#include "lpc1768/tm/tm.h"
#include "lpc1768/rtc/rtc.h"

#define GPREG1 (*((volatile unsigned *) 0x40024048))

/*
+----+----+----+----+----+----+----+----+
|Flgs|  Leap months |     Leap count    |
|UUDE|    12 bits   |      16 bits      |
+----+----+----+----+----+----+----+----+

Leap months: 12 bits will hold 4096 months or 341 years

Leap count: 16 bits will hold enough leaps seconds for 60,000 years

Flgs
U = unused
D = direction: 1 to subtract; 0 to add
E = Enable: 1 if leap to take into account at the start of the leap month; 0 if already taken or to be ignored
*/

//Leap seconds
static int     epochOffset   = 0; //12 bits holds enough leap seconds for at least 300 years.
static clktime epochOffset64 = 0;
int  ClkUtcGetEpochOffset() { return epochOffset; }
void ClkUtcSetEpochOffsetWithUtcChange(int value)
{
    epochOffset   = value;
    epochOffset64 = (clktime)epochOffset << CLK_TIME_ONE_SECOND_SHIFT;
    GPREG1 = (GPREG1 & 0xFFFF0000) | (value & 0x0000FFFF);
}
void ClkUtcAddEpochOffsetWithUtcChange(int value)
{
    ClkUtcSetEpochOffsetWithUtcChange(epochOffset + value);
}
void ClkUtcSetEpochOffsetWithoutUtcChange(int value)
{
    ClkTimeAdjustSeconds(value - epochOffset); //Adjust the tai time by the difference
    ClkUtcSetEpochOffsetWithUtcChange(value);
}

//Next leap second
static int     nextEpochMonth1970 = 0;
static clktime nextEpochUtc       = 0;
int     ClkUtcGetNextEpochMonth1970() { return nextEpochMonth1970; }
clktime ClkUtcGetNextEpoch         () { return nextEpochUtc;       }
static void makeNextEpochUtc()
{
    int year  = nextEpochMonth1970 / 12 + 1970;
    int month = nextEpochMonth1970 % 12 + 1;
    struct tm tm;
    TmFromInteger(year, month, 1, 0, 0, 0, &tm);
    time64 t = TmUtcToTime64(&tm);
    nextEpochUtc = (clktime)t << CLK_TIME_ONE_SECOND_SHIFT;
}
void ClkUtcSetNextEpochMonth1970(int value)
{
    nextEpochMonth1970 = value;
    makeNextEpochUtc();
    GPREG1 = (GPREG1 & 0xF000FFFF) | (((uint32_t)value << 16) & 0x0FFF0000); //Precedence order: shifts then ands then ors.
}

static bool nextLeapEnable  = false;
static bool nextLeapForward = true;
bool ClkUtcGetNextLeapEnable () { return nextLeapEnable;  }
bool ClkUtcGetNextLeapForward() { return nextLeapForward; }
void ClkUtcSetNextLeapEnable(bool value)
{
    nextLeapEnable = value;
    if (value) GPREG1 |= 0x10000000;
    else       GPREG1 &= 0xEFFFFFFF;
}
void ClkUtcSetNextLeapForward(bool value)
{
    nextLeapForward = value;
    if (value) GPREG1 |= 0x20000000;
    else       GPREG1 &= 0xDFFFFFFF;
}
void ClkUtcTglNextLeapEnable () { ClkUtcSetNextLeapEnable (!nextLeapEnable ); }
void ClkUtcTglNextLeapForward() { ClkUtcSetNextLeapForward(!nextLeapForward); }

void ClkUtcInit(void)
{
    if (RtcPowerLost()) GPREG1 = 0;
    
    epochOffset         =  GPREG1 & 0x0000FFFF;
    epochOffset64       = (clktime)epochOffset << CLK_TIME_ONE_SECOND_SHIFT;
    nextEpochMonth1970  = (GPREG1 & 0x0FFF0000) >> 16;
    makeNextEpochUtc();
    nextLeapEnable      =  GPREG1 & 0x10000000;
    nextLeapForward     =  GPREG1 & 0x20000000;
}


clktime ClkUtcFromTai(clktime tai) { return tai - epochOffset64; }
clktime ClkUtcToTai  (clktime utc) { return utc + epochOffset64; }

void    ClkUtcCheckAdjustLeapSecondCount(clktime tai)
{
    if (!nextLeapEnable) return; //Do nothing if leaps are disabled
    
    clktime utc      = ClkUtcFromTai(tai);
    clktime epochEnd = ClkUtcGetNextEpoch() - (nextLeapForward ? 0 : 1);
    
    if (utc < epochEnd) return; //Do nothing until reached the end of the current epoch
    
    if (nextLeapForward) ClkUtcAddEpochOffsetWithUtcChange(+1); //repeat 59
    else                 ClkUtcAddEpochOffsetWithUtcChange(-1); //skip   59
    
    ClkUtcSetNextLeapEnable(false);
    
}