#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "log/log.h"
#include "net/net.h"
#include "net/eth/mac.h"
#include "net/ip6/ip6addr.h"
#include "net/ip6/slaac.h"
#include "net/ip6/icmp/ndp/rs.h"
#include "lpc1768/mstimer/mstimer.h"

int      NdpHopLimit             = 0;
bool     NdpManagedConfiguration = false;
bool     NdpOtherConfiguration   = false;

char     NdpRouterMac[6];


int      NdpGlobalPrefixLength            = 0;
bool     NdpGlobalPrefixFlagL             = false;
bool     NdpGlobalPrefixFlagA             = false;
uint32_t NdpGlobalPrefixValidLifetime     = 0;
uint32_t NdpGlobalPrefixPreferredLifetime = 0;
char     NdpGlobalPrefix[16];

int      NdpUniqueLocalPrefixLength            = 0;
bool     NdpUniqueLocalPrefixFlagL             = false;
bool     NdpUniqueLocalPrefixFlagA             = false;
uint32_t NdpUniqueLocalPrefixValidLifetime     = 0;
uint32_t NdpUniqueLocalPrefixPreferredLifetime = 0;
char     NdpUniqueLocalPrefix[16];

uint32_t NdpDnsLifetime = 0;
char     NdpDnsServer[16];

int      NdpMtu = 0;

static uint32_t elapsedLifeMsTimer = 0;

uint32_t NdpGetElapsedLife()
{
    return (MsTimerCount - elapsedLifeMsTimer) / 1000;
}
static int  lease = 0;
int  NdpGetLease() { return lease; }
void NdpSetLease(int value)         //Set whenever an IP address request has been acknowledged
{
    lease = value;
    elapsedLifeMsTimer = MsTimerCount;
}
bool NdpIsFresh()
{
    uint32_t elapsedLifeMs = MsTimerCount - elapsedLifeMsTimer;
    uint32_t leaseMs = lease * 1000;

    if (lease && elapsedLifeMs < (leaseMs >> 1)) return true;  //Fresh if within half the lease

    if (!lease || elapsedLifeMs >= leaseMs)
    {
        if (lease)
        {
            if (NetTraceNewLine) Log("\r\n");
            LogTime("NDP lease has expired -> clearing NDP information\r\n");
        }
        elapsedLifeMsTimer = MsTimerCount;
        Ip6AddrClear(SlaacUniqueLocalIp);
        Ip6AddrClear(SlaacGlobalIp);
        Ip6AddrClear(NdpDnsServer);
    }
    
    return false;
}

static uint32_t decodeUint32(char* p)
{
    uint32_t     value  = *p++;
    value <<= 8; value += *p++;
    value <<= 8; value += *p++;
    value <<= 8; value += *p++;
    return value;
}
static int decodeOption(char* p, char* srcMac, char* dstMac)
{
    int type = *p++;
    int size = *p++;
    if (size == 0) return 0;
    switch (type)
    {
        case 1:
            if (srcMac) MacCopy(srcMac, p);
            break;
        case 2:
            if (dstMac) MacCopy(dstMac, p);
            break;
        case 3:
            {
                int      length = *p;                               p += 1;
                uint8_t   flags = *p;                               p += 1;
                uint32_t validLifetime     = decodeUint32(p);       p += 4;
                uint32_t preferredLifetime = decodeUint32(p);       p += 4;
                /*Ignore the reserved2 field*/                      p += 4;
                if (Ip6AddrIsGlobal(p))
                {
                    Ip6AddrCopy(NdpGlobalPrefix,                   p);
                                NdpGlobalPrefixLength            = length;
                                NdpGlobalPrefixFlagL             = flags & 0x80;
                                NdpGlobalPrefixFlagA             = flags & 0x40;
                                NdpGlobalPrefixValidLifetime     = validLifetime;
                                NdpGlobalPrefixPreferredLifetime = preferredLifetime;
                                SlaacMakeGlobal(p);
                }
                else if (Ip6AddrIsUniqueLocal(p))
                {
                    Ip6AddrCopy(NdpUniqueLocalPrefix,                   p);
                                NdpUniqueLocalPrefixLength            = length;
                                NdpUniqueLocalPrefixFlagL             = flags & 0x80;
                                NdpUniqueLocalPrefixFlagA             = flags & 0x40;
                                NdpUniqueLocalPrefixValidLifetime     = validLifetime;
                                NdpUniqueLocalPrefixPreferredLifetime = preferredLifetime;
                                SlaacMakeUniqueLocal(p);
                }
                break;
            }
        case 5:
            /*Ignore the reserved field*/                           p += 2;
            NdpMtu = decodeUint32(p);
            break;
        case 25:
            /*Ignore the reserved field*/                           p += 2;
            NdpDnsLifetime = decodeUint32(p);                       p += 4;
            Ip6AddrCopy(NdpDnsServer, p);
            break;
    }
    return size * 8;
}
static void logFlagsLA(char flags)
{
    if (flags & 0x80) LogChar('L');
    if (flags & 0x40) LogChar('A');
}
static int logOptionVerbose(char* p)
{
    uint32_t value;
    int type = *p++;
    int size = *p++;
    if (size == 0)
    {
        LogF("    Size zero for option %d\r\n", type);
        return 0;
    }
    switch (type)
    {
        case 1:
                                           Log("    Src MAC          "); MacLog(p);         Log("\r\n"); break;
        case 2:
                                           Log("    Tgt MAC          "); MacLog(p);         Log("\r\n"); break;
        case 3:
                                           Log("    Prefix length    "); LogF("%d", *p);    Log("\r\n"); p += 1;
                                           Log("    Prefix flags     "); logFlagsLA(*p);    Log("\r\n"); p += 1;
            value = decodeUint32(p);       Log("    Prefix valid     "); LogF("%u", value); Log("\r\n"); p += 4;
            value = decodeUint32(p);       Log("    Prefix preferred "); LogF("%u", value); Log("\r\n"); p += 4;
            /*Ignore the Reserved2 field*/                                                               p += 4;
                                           Log("    Prefix           "); Ip6AddrLog(p);  Log("\r\n"); break;
        case 5:
            /*Ignore the reserved field*/                                                                p += 2;
            value = decodeUint32(p);       Log("    MTU              "); LogF("%u", value); Log("\r\n"); break;
        case 25:
            /*Ignore the reserved field*/                                                                p += 2;
            value = decodeUint32(p);       Log("    DNS lifetime     "); LogF("%u", value); Log("\r\n"); p += 4;
                                           Log("    DNS Server       "); Ip6AddrLog(p);  Log("\r\n"); break;
        default:
                                           Log("    Unknown option   "); LogF("%d", type);  Log("\r\n"); break;
    }
    return size * 8;
}
static int logOptionQuiet(char* p)
{
    uint32_t value;
    int type = *p++;
    int size = *p++;
    if (size == 0) return 0;
    switch (type)
    {
        case 1:
            Log(" src ");
            MacLog(p);
            break;
        case 2:
            Log(" tgt ");
            MacLog(p);
            break;
        case 3:
            p++; //Length
            p++; //LA
            p += 4; //Valid lifetime
            p += 4; //Preferred lifetime
            p += 4; //Reserved 2
            Log(" prefix ");
            Ip6AddrLog(p); //IP6 address
            break;
        case 5:
            p += 2; //Skip past the reserved field
            value = decodeUint32(p);
            p += 4;
            LogF(" MTU %u", value);
            break;
        case 25:
            p += 2; //Skip past the reserved field
            p += 4; //DNS lifetime
            Log(" DNS ");
            Ip6AddrLog(p);
            break;
        default:
            LogF(" ? %d", type);
            break;
    }
    return size * 8;
}
void NdpDecodeOptions(char* pData, int dataLength, char* srcMac, char* dstMac)
{
    char* p = pData;
    char* pE = pData + dataLength;
    while(p < pE)
    {
        int size = decodeOption(p, srcMac, dstMac);
        if (size == 0) break;
        p += size;
    }
}
void NdpLogOptionsVerbose(char* pData, int dataLength)
{
    char* p = pData;
    char* pE = pData + dataLength;
    while(p < pE)
    {
        int size = logOptionVerbose(p);
        if (size == 0) break;
        p += size;
    }
}
void NdpLogOptionsQuiet(char* pData, int dataLength)
{
    char* p = pData;
    char* pE = pData + dataLength;
    while(p < pE)
    {
        int size = logOptionQuiet(p);
        if (size == 0) break;
        p += size;
    }
}
int NdpAddOptionSourceMac(char* p, char* pMac)
{
    *p++ = 1; //Source MAC option
    *p++ = 1; //8 bytes
    MacCopy(p, pMac);
    return 8;
}
int NdpAddOptionTargetMac(char* p, char* pMac)
{
    *p++ = 2; //Target MAC option
    *p++ = 1; //8 bytes
    MacCopy(p, pMac);
    return 8;
}
