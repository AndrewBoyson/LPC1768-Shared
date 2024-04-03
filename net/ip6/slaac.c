#include <string.h>

#include "net/eth/mac.h"
#include "net/ip6/ip6addr.h"

char SlaacLinkLocalIp[16];
char SlaacUniqueLocalIp[16];
char SlaacGlobalIp[16];

int SlaacScope(char* ip)
{
    if (Ip6AddrIsSame(ip, SlaacLinkLocalIp)) return SCOPE_LINK_LOCAL;
    if (Ip6AddrIsSame(ip, SlaacGlobalIp   )) return SCOPE_GLOBAL;
    return SCOPE_NONE;
}
void SlaacAddressFromScope(int scope, char* pSrcIp)
{
    switch (scope)
    {
        case SCOPE_GLOBAL:       Ip6AddrCopy(pSrcIp, SlaacGlobalIp     ); break;
        case SCOPE_UNIQUE_LOCAL: Ip6AddrCopy(pSrcIp, SlaacUniqueLocalIp); break;
        default:                 Ip6AddrCopy(pSrcIp, SlaacLinkLocalIp  ); break;
    }
    //Note that scope could be SCOPE_NONE if source was multicast in which case should return the link local ip.
}

void SlaacMakeGlobal(char* pPrefix)
{
    memcpy(SlaacGlobalIp, pPrefix, 8);
    char* p = SlaacGlobalIp + 8;
    *p++ = MacLocal[0] | 0x02; //Modified EUI-64
    *p++ = MacLocal[1];
    *p++ = MacLocal[2];
    *p++ = 0xFF;
    *p++ = 0xFE;
    *p++ = MacLocal[3];
    *p++ = MacLocal[4];
    *p++ = MacLocal[5];
    
}
void SlaacMakeUniqueLocal(char* pPrefix)
{
    memcpy(SlaacUniqueLocalIp, pPrefix, 8);
    char* p = SlaacUniqueLocalIp + 8;
    *p++ = MacLocal[0] | 0x02; //Modified EUI-64
    *p++ = MacLocal[1];
    *p++ = MacLocal[2];
    *p++ = 0xFF;
    *p++ = 0xFE;
    *p++ = MacLocal[3];
    *p++ = MacLocal[4];
    *p++ = MacLocal[5];
    
}
void SlaacInit()
{
    char* p = SlaacLinkLocalIp; //fe80::::202:f7ff:fef2:7d27
    *p++ = 0xFE;
    *p++ = 0x80;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = MacLocal[0] | 0x02; //Modified EUI-64
    *p++ = MacLocal[1];
    *p++ = MacLocal[2];
    *p++ = 0xFF;
    *p++ = 0xFE;
    *p++ = MacLocal[3];
    *p++ = MacLocal[4];
    *p++ = MacLocal[5];
}