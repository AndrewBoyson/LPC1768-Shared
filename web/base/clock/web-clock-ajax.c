#include  <stdint.h>
#include   <stdio.h>

#include "web/http/http.h"
#include "lpc1768/rtc/rtc.h"
#include "lpc1768/scan/scan.h"
#include "clk/clk.h"
#include "clk/clktime.h"
#include "clk/clkgov.h"
#include "clk/clkutc.h"
#include "net/udp/ntp/ntpclient.h"

void WebClockAjax()
{
    HttpOk("text/plain; charset=UTF-8", "no-cache", NULL, NULL);
    
    //Time and UTC
    clktime now = ClkNowTai();
    clktime fraction = now & ((1UL << CLK_TIME_ONE_SECOND_SHIFT) - 1);
    clktime ms = (fraction * 1000) >> CLK_TIME_ONE_SECOND_SHIFT;
    HttpAddInt16AsHex(ms                           ); HttpAddChar('\n');
    char byte = 0;
    if (RtcIsSet()                ) byte |= 0x01;
    if (ClkTimeIsSet()            ) byte |= 0x02;
    if (ClkGovIsReceivingTime     ) byte |= 0x04;
    if (ClkGovRateIsSynced        ) byte |= 0x08;
    if (ClkGovTimeIsSynced        ) byte |= 0x10;
    if (ClkUtcGetNextLeapEnable() ) byte |= 0x20;
    if (ClkUtcGetNextLeapForward()) byte |= 0x40;
    if (ClkGovTrace)                byte |= 0x80;
    HttpAddByteAsHex (byte                         ); HttpAddChar('\n');
    HttpAddInt12AsHex(ClkUtcGetNextEpochMonth1970()); HttpAddChar('\n');
    HttpAddInt16AsHex(ClkUtcGetEpochOffset()       ); HttpAddChar('\n');
    HttpAddChar('\f');
    
    //Governer
    HttpAddInt32AsHex(ClkGovGetPpb()               ); HttpAddChar('\n');
    HttpAddInt32AsHex(ClkGovFreqDivisor            ); HttpAddChar('\n');
    HttpAddInt32AsHex(ClkGovFreqChangeMaxPpb       ); HttpAddChar('\n');
    HttpAddInt32AsHex(ClkGovFreqSyncedLimPpb       ); HttpAddChar('\n');
    HttpAddInt32AsHex(ClkGovFreqSyncedHysPpb       ); HttpAddChar('\n');
    HttpAddInt32AsHex(ClkGovSlewDivisor            ); HttpAddChar('\n');
    HttpAddInt32AsHex(ClkGovSlewChangeMaxMs        ); HttpAddChar('\n');
    HttpAddInt32AsHex(ClkGovSlewSyncedLimNs        ); HttpAddChar('\n');
    HttpAddInt32AsHex(ClkGovSlewSyncedHysNs        ); HttpAddChar('\n');
    HttpAddInt32AsHex(ClkGovSlewOffsetMaxSecs      ); HttpAddChar('\n');
    HttpAddChar('\f');
    
    //NTP
    HttpAddText      (NtpClientQueryServerName     ); HttpAddChar('\n');
    HttpAddInt32AsHex(NtpClientQueryInitialInterval); HttpAddChar('\n');
    HttpAddInt32AsHex(NtpClientQueryNormalInterval ); HttpAddChar('\n');
    HttpAddInt32AsHex(NtpClientQueryRetryInterval  ); HttpAddChar('\n');
    HttpAddInt32AsHex(NtpClientReplyOffsetMs       ); HttpAddChar('\n');
    HttpAddInt32AsHex(NtpClientReplyMaxDelayMs     ); HttpAddChar('\n');
    HttpAddChar('\f');
    
    //Scan
    HttpAddInt32AsHex(ScanAverage                  ); HttpAddChar('\n');
    HttpAddInt32AsHex(ScanMaximum                  ); HttpAddChar('\n');
    HttpAddInt32AsHex(ScanMinimum                  ); HttpAddChar('\n');
    HttpAddChar('\f');
}

