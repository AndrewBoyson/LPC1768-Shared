#include <stdint.h>

#include "ntpclient.h"
#include "ntptimestamp.h"
#include "ntphdr.h"
#include "clock/clk/clktime.h"
#include "clock/clk/clktm.h"
#include "clock/clk/clkutc.h"
#include "clock/clk/clkgov.h"
#include "log/log.h"
#include "net/net.h"

int32_t NtpClientReplyOffsetMs;                           
int32_t NtpClientReplyMaxDelayMs;   

void NtpClientReply(void (*traceback)(void), char* pPacket)
{
    if (NtpTrace)
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTimeF("NTP received reply\r\n");
        if (NetTraceStack) traceback();
        NtpLogHeader(pPacket);
    }
    uint64_t ori = NtpHdrGetOriTimeStamp(pPacket);
    if (NtpClientQueryTime == 0 || ori != NtpClientQueryTime)
    {
        LogTimeF("NtpClient error: unsolicited reply\r\n");
        return;
    }
    
    uint64_t rec = NtpHdrGetRecTimeStamp(pPacket);
    int      li  = NtpHdrGetLI          (pPacket);
    
    //Check the received timestamp delay
    clktime oriTicks = NtpTimeStampToClkTime(ori);
    clktime ntpTicks = NtpTimeStampToClkTime(rec);
    clktime clkTicks = ClkTimeGet();  //Use the costly time this instant
    
    clktime roundTripTicks = clkTicks - oriTicks;
    clktime delayMs        = roundTripTicks >> CLK_TIME_ONE_MS_ISH_SHIFT;
    clktime limit          = NtpClientReplyMaxDelayMs;
    if (delayMs > limit)
    {
        LogTimeF("NtpClient error: delay %lld ms is greater than limit %lld ms\r\n", delayMs, limit);
        return; 
    }
    
    if (NtpClientTrace)
    {
        int64_t diffMs = (ntpTicks - clkTicks) >> CLK_TIME_ONE_MS_ISH_SHIFT;
        LogTimeF("NtpClient difference (ext-int) is %lld ms\r\n", diffMs);
    }
    
    //Handle the LI
    if (li == 3) 
    {
        LogTimeF("NtpClient error: NTP server is not synchronised (LI = 3)\r\n");
        return; 
    }
    if (li == 1 || li == 2)
    {
        struct tm tm;
        ClkTimeToTmUtc(clkTicks, &tm);
        int year1970 = tm.tm_year - 70; //1900
        int month    = tm.tm_mon;       //0 to 11
        ClkUtcSetNextEpochMonth1970(year1970 * 12 + month + 1); //+1 as new UTC epoch is at the start of next month
        ClkUtcSetNextLeapForward(li == 1);
        ClkUtcSetNextLeapEnable(true);
    }
    if (li == 0)
    {
        ClkUtcSetNextLeapEnable(false);
    }
    
    //Set the clock
    clktime offsetTime = NtpClientReplyOffsetMs << CLK_TIME_ONE_MS_ISH_SHIFT;
    ClkGovSyncTime(ntpTicks + offsetTime);
    
    //Tell the query service that the time has been updated
    NtpClientTimeUpdateSuccessful();
}
