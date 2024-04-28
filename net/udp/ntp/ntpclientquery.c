#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "log/log.h"
#include "clk/clk.h"
#include "clk/clktime.h"
#include "clk/clkutc.h"
#include "clk/clkgov.h"
#include "clk/clktm.h"
#include "lpc1768/mstimer/mstimer.h"
#include "ntptimestamp.h"
#include "net/net.h"
#include "ntp.h"
#include "ntphdr.h"
#include "ntpclient.h"
#include "net/udp/dns/dns.h"
#include "net/ip4/ip4.h"
#include "net/resolve/resolve.h"
#include "net/action.h"
#include "net/udp/dns/dnslabel.h"

#define ONE_BILLION 1000000000ULL
#define ONE_MILLION     1000000LL

char    NtpClientQueryServerName[DNS_MAX_LABEL_LENGTH+1]; 
int32_t NtpClientQueryInitialInterval;                    
int32_t NtpClientQueryNormalInterval;                     
int32_t NtpClientQueryRetryInterval;                      

bool       NtpClientQuerySendRequestsViaIp4 = false;
uint32_t   NtpClientQueryServerIp4;
char       NtpClientQueryServerIp6[16];

static uint64_t startNtpMs = 0;
static int  intervalTypeNtp = NTP_QUERY_INTERVAL_INITIAL;
static bool intervalCompleteNtp()
{
    uint32_t interval = 0;
    switch(intervalTypeNtp)
    {
        case NTP_QUERY_INTERVAL_INITIAL: interval = NtpClientQueryInitialInterval; break;
        case NTP_QUERY_INTERVAL_NORMAL:  interval = NtpClientQueryNormalInterval;  break;
        case NTP_QUERY_INTERVAL_RETRY:   interval = NtpClientQueryRetryInterval;   break;
    }
    return MsTimerRelative(startNtpMs, interval * 1000);
}
void NtpClientQueryStartInterval(int type)
{
    startNtpMs = MsTimerCount;
    intervalTypeNtp = type;
}

void writeRequest(char* pPacket, int* pSize)
{   
    NtpHdrSetMode(pPacket, NTP_CLIENT);
    NtpHdrSetVersion     (pPacket, 3);
    NtpHdrSetLI          (pPacket, 0);
    NtpHdrSetStratum     (pPacket, 0);
    NtpHdrSetPoll        (pPacket, 0);
    NtpHdrSetPrecision   (pPacket, 0);
    NtpHdrSetRootDelay   (pPacket, 0);
    NtpHdrSetDispersion  (pPacket, 0);
    char* p = NtpHdrPtrRefIdentifier(pPacket);
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p   = 0;
    NtpHdrSetRefTimeStamp(pPacket, 0);
    NtpHdrSetOriTimeStamp(pPacket, 0);
    NtpHdrSetRecTimeStamp(pPacket, 0);
    NtpClientQueryTime = NtpTimeStampFromClkTime(ClkTimeGet()); //Record the time to filter replies; use the costly time
    NtpHdrSetTraTimeStamp(pPacket, NtpClientQueryTime);

    *pSize = NTP_HEADER_LENGTH;
}

int NtpClientQueryPoll(int type, char* pPacket, int* pSize)
{
    if (NtpClientQueryServerName[0]) //An empty name means ntp client is not enabled
    {
        if (intervalCompleteNtp()) //Wait for the time out
        {
            bool isMulticast = NtpClientQueryServerName[0] == '*';
            if (isMulticast || Resolve(NtpClientQueryServerName, type, &NtpClientQueryServerIp4, NtpClientQueryServerIp6))
            {
                ClkGovIsReceivingTime = false;
                NtpClientQueryStartInterval(NTP_QUERY_INTERVAL_RETRY);
                writeRequest(pPacket, pSize);
                
                if (isMulticast) return MULTICAST_NTP;
                else             return   UNICAST_NTP;
            }
        }
    }
    return DO_NOTHING;
}