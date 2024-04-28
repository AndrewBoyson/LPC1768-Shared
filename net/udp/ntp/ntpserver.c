#include <stdint.h>
#include <stdlib.h>

#include "clk/clk.h"
#include "clk/clkgov.h"
#include "clk/clktime.h"
#include "clk/clkutc.h"
#include "ntptimestamp.h"
#include "ntp.h"
#include "ntphdr.h"
#include "net/action.h"
#include "log/log.h"
#include "net/net.h"

#define LI_PRIOR_NOTICE_SECONDS 10LL * 24 * 60 * 60

bool NtpServerEnable  = false;

static int getPrecision()
{
    /*
    The Precision field is set to reflect the maximum reading error of the local clock.
    For all practical cases it is computed as the negative of the number of significant bits
    to the right of the decimal point in the NTP timestamp format.
    While that could be CLK_TIME_ONE_SECOND_SHIFT I think that would be the resolution not the precision
    I therefore assume I can count on the clock to be good to about a microsecond or 2^-20
    */
    return -20;
}
static int getLi()
{
    if (!ClkTimeIsSet()           ) return 3; //Alarm condition; clock is not synchronised
    if (!ClkUtcGetNextLeapEnable()) return 0; //No warning
    
    clktime nowTime = ClkUtcFromTai(ClkNowTai()); //Use the less costly time this scan
    clktime liPriorNotice = LI_PRIOR_NOTICE_SECONDS << CLK_TIME_ONE_SECOND_SHIFT;
    
    if (nowTime > ClkUtcGetNextEpoch() - liPriorNotice)
    {
        if (ClkUtcGetNextLeapForward()) return 1; //Last minute has 61 seconds
        else                            return 2; //Last minute has 59 seconds
    }
    else
    {
        return 0;                             //No warning
    }
}

static int getStratum()
{
    if (ClkTimeIsSet()) return 1;
    else                return 0;
}
static char* getIdent()
{
    if (!ClkTimeIsSet()  ) return "INIT";
    if (!ClkGovIsSynced()) return "LOCL";
    return NetName;
}
static uint64_t getRefNtp()
{
    return NtpTimeStampFromClkTime(ClkRefTai());
}
int NtpServerRequest(void (*traceback)(void), char* pPacketRx, char* pPacketTx)
{
    if (!NtpServerEnable) return DO_NOTHING;
    
    if (NtpTrace)
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTimeF("NTP received request\r\n");
        if (NetTraceStack) traceback();
        NtpLogHeader(pPacketRx);
    }

    uint64_t refNtp    = getRefNtp();
    uint64_t nowNtp    = NtpTimeStampFromClkTime(ClkTimeGet()); //use the costly time at this instant
    int      stratum   = getStratum();
    char*    ident     = getIdent();
    int      li        = getLi();
    int      precision = getPrecision();

    NtpHdrSetMode        (pPacketTx, NTP_SERVER);
    NtpHdrSetVersion     (pPacketTx, NtpHdrGetVersion(pPacketRx));
    NtpHdrSetLI          (pPacketTx, li);
    NtpHdrSetStratum     (pPacketTx, stratum);
    NtpHdrSetPoll        (pPacketTx, NtpHdrGetPoll(pPacketRx));
    NtpHdrSetPrecision   (pPacketTx, precision);
    NtpHdrSetRootDelay   (pPacketTx, 0);
    NtpHdrSetDispersion  (pPacketTx, 0);
    char* p = NtpHdrPtrRefIdentifier(pPacketTx);
    *p++ = ident[0]; //For stratum 1 (reference clock), this is a four-octet, left-justified, zero-padded ASCII string.
    *p++ = ident[1];
    *p++ = ident[2];
    *p   = ident[3];
    NtpHdrSetRefTimeStamp(pPacketTx, refNtp);
    NtpHdrSetOriTimeStamp(pPacketTx, NtpHdrGetTraTimeStamp(pPacketRx));
    NtpHdrSetRecTimeStamp(pPacketTx, nowNtp);
    NtpHdrSetTraTimeStamp(pPacketTx, nowNtp);
    
    if (NtpTrace) NtpLogHeader(pPacketTx);
    return UNICAST;
}

