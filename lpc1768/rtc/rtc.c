#include <time.h>
#include <stdbool.h>

#include "lpc1768/register.h"

bool  RtcIsSet()     { return !(AUX & 0x10); } //27.6.2.5 RTC Auxiliary control register - RTC Oscillator Fail detect flag
bool  RtcPowerLost() { return   AUX & 0x10 ; } //27.6.2.5 RTC Auxiliary control register - RTC Oscillator Fail detect flag

void RtcGetTm(struct tm* ptm)
{
    ptm->tm_sec    = SEC;
    ptm->tm_min    = MIN;
    ptm->tm_hour   = HOUR;
    ptm->tm_mday   = DOM;
    ptm->tm_mon    = MONTH - 1;
    ptm->tm_year   = YEAR  - 1900;
    ptm->tm_wday   = DOW;
    ptm->tm_yday   = DOY   - 1;
    ptm->tm_isdst  = -1; // -ve should signify that dst data is not available but it is used here to denote UTC
}
void RtcSetTm(struct tm* ptm)
{
    CCR     = 2;                   //Stop and reset the divider (CLKEN bit 0 = 0, CTCRST bit 1 = 1)
    SEC     = ptm->tm_sec;         // 0 --> 59
    MIN     = ptm->tm_min;         // 0 --> 59
    HOUR    = ptm->tm_hour;        // 0 --> 23
    DOM     = ptm->tm_mday;        // 1 --> 31
    MONTH   = ptm->tm_mon  + 1;    // rtc    1 -->   12; tm 0 -->  11
    YEAR    = ptm->tm_year + 1900; // rtc 1900 --> 2100; tm 0 --> 200
    DOW     = ptm->tm_wday;        // 0 --> 6 where 0 == Sunday
    DOY     = ptm->tm_yday + 1;    // rtc 1 --> 366;     tm 0 --> 365 
    AUX     = 0x10;                //27.6.2.5 RTC Auxiliary control register - RTC Oscillator Fail detect flag - Writing a 1 to this bit clears the flag.
    CCR     = 1;                   //Release the divider (CLKEN bit 0 = 1, CTCRST bit 1 = 0)
}
void RtcInit()
{
    PCONP |= 1 <<  9; //RTC
	
	CALIBRATION =    0; //27.6.4.2 Calibration register - disabled if 0 even if enable calibration counter (CCALEN bit 4 = 0)
    CCR         =    1; //27.6.2.2 Clock Control Register - enable the time counters (CLKEN bit 0 = 1); no reset (CTCRST bit 1 = 0)
    CIIR        =    0; //27.6.2.3 Counter Increment Interrupt Register - set to not interrupt
    AMR         = 0xFF; //27.6.2.4 Alarm Mask Register - mask all alarms
    AUXEN       =    0; //27.6.2.6 RTC Auxiliary Enable register - mask the oscillator stopped interrupt
    ILR         =    3; //27.6.2.1 Interrupt Location Register - Clear all interrupts
}
