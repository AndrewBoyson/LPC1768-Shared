#include <stdint.h>
#include <stdbool.h>

#include "log/log.h"
#include "lpc1768/mstimer/mstimer.h"
#include "net/net.h"
#include "net/action.h"
#include "net/ip4/ip4addr.h"
#include "net/ip6/ip6addr.h"
#include "net/udp/dhcp/dhcp.h"
#include "dns.h"
#include "net/udp/udp.h"
#include "net/eth/eth.h"
#include "net/ip6/slaac.h"
#include "dnshdr.h"
#include "dnsname.h"
#include "dnslabel.h"
#include "net/resolve/ar6.h"
#include "net/ip6/icmp/ndp/ndp.h"
#include "net/eth/mac.h"

bool DnsQueryTrace = false;

#define TIME_OUT_SENT_MS 3000

#define MDNS_UNICAST false

char     DnsQueryName[DNS_MAX_LABEL_LENGTH+1];
uint32_t DnsQueryIp4 = 0;
char     DnsQueryIp6[16];
bool     DnsQueryIsBusy     = false;

static char     _RecordType  = DNS_RECORD_NONE;
static int      _DnsProtocol = DNS_PROTOCOL_NONE;
static int      _IpProtocol  = 0;
static uint32_t _StartedMs   = 0;

static void reap()
{
    if (!DnsQueryIsBusy) return;
    
    if (MsTimerRelative(_StartedMs, TIME_OUT_SENT_MS))
    {
        LogTimeF("DnsQuery reaped ongoing request for record type ");
        DnsRecordTypeLog(_RecordType);
        if (DnsQueryName[0]) { LogF(" name '%s'", DnsQueryName); }
        if (DnsQueryIp4)     { Log(" address "); Ip4AddrLog(DnsQueryIp4); }
        if (DnsQueryIp6[0])  { Log(" address "); Ip6AddrLog(DnsQueryIp6); }
        Log(" using ");
        DnsProtocolLog(_DnsProtocol);
        Log(" over ");
        EthProtocolLog(_IpProtocol);
        LogF("\r\n");
        
        DnsQueryName[0]    = 0;
        DnsQueryIp4        = 0;
        DnsQueryIp6[0]     = 0;
        DnsQueryIsBusy     = false;
        _StartedMs         = MsTimerCount;
        _DnsProtocol       = DNS_PROTOCOL_NONE;
        _IpProtocol        = 0;
        _RecordType        = DNS_RECORD_NONE;
    }
}
void DnsQueryMain()
{
    reap();
}
void DnsQueryIp4FromName(char * name, int dnsProtocol, int ipProtocol)
{
    if (!name[0])
    {
        LogTime("DnsQueryIp4FromName called with no name\r\n");
        return;
    }
    DnsLabelMakeFullNameFromName(dnsProtocol, name, sizeof(DnsQueryName), DnsQueryName);
    DnsQueryIp4     = 0;
    DnsQueryIp6[0]  = 0;
    DnsQueryIsBusy  = true;
    _StartedMs      = MsTimerCount;
    _DnsProtocol    = dnsProtocol;
    _IpProtocol     = ipProtocol;
    _RecordType     = DNS_RECORD_A;
}
void DnsQueryIp6FromName(char * name, int dnsProtocol, int ipProtocol)
{
    if (!name[0])
    {
        LogTime("DnsQueryIp6FromName called with no name\r\n");
        return;
    }
    DnsLabelMakeFullNameFromName(dnsProtocol, name, sizeof(DnsQueryName), DnsQueryName);
    DnsQueryIp4     = 0;
    DnsQueryIp6[0]  = 0;
    DnsQueryIsBusy  = true;
    _StartedMs      = MsTimerCount;
    _DnsProtocol    = dnsProtocol;
    _IpProtocol     = ipProtocol;
    _RecordType     = DNS_RECORD_AAAA;
}
void DnsQueryNameFromIp4(uint32_t ip, int dnsProtocol, int ipProtocol)
{
    if (!ip)
    {
        LogTime("DnsQueryNameFromIp4 called with no ip\r\n");
        return;
    }
    DnsQueryName[0] = 0;
    DnsQueryIp4     = ip;
    DnsQueryIp6[0]  = 0;
    DnsQueryIsBusy  = true;
    _StartedMs      = MsTimerCount;
    _DnsProtocol    = dnsProtocol;
    _IpProtocol     = ipProtocol;
    _RecordType     = DNS_RECORD_PTR;
}
void DnsQueryNameFromIp6(char* ip, int dnsProtocol, int ipProtocol)
{
    if (!ip[0])
    {
        LogTime("DnsQueryNameFromIp6 called with no ip\r\n");
        return;
    }
    DnsQueryName[0] = 0;
    DnsQueryIp4     = 0;
    Ip6AddrCopy(DnsQueryIp6, ip);
    DnsQueryIsBusy  = true;
    _StartedMs      = MsTimerCount;
    _DnsProtocol    = dnsProtocol;
    _IpProtocol     = ipProtocol;
    _RecordType     = DNS_RECORD_PTR;
}
static void logQuery()
{
    if (NetTraceNewLine) Log("\r\n");
    LogTimeF("DnsQuery sent ");
    DnsProtocolLog(_DnsProtocol);
    Log(" request for ");
    DnsRecordTypeLog(_RecordType);
    Log(" ");
    if (DnsQueryIp4) //Reverse
    {
        Ip4AddrLog(DnsQueryIp4);
    }
    else if (DnsQueryIp6[0])
    {
        Ip6AddrLog(DnsQueryIp6);
    }
    else //Forward
    {
        Log(DnsQueryName);
    }
    Log("\r\n");
}
int DnsQueryPoll(int ipType, void* pPacket, int* pSize)
{   
    DnsHdrSetup(pPacket, *pSize);

    if (!DnsQueryIsBusy)                                       return DO_NOTHING;
    if (_IpProtocol  != EthProtocol)                           return DO_NOTHING; //Only use a poll from the required protocol
    if (_DnsProtocol == DNS_PROTOCOL_UDNS && DhcpLocalIp == 0) return DO_NOTHING;
    if (_RecordType  == DNS_RECORD_NONE)                       return DO_NOTHING;
    if (_DnsProtocol == DNS_PROTOCOL_NONE)                     return DO_NOTHING;
    
    
    //For IPv6 UDNS check if have the MAC for the DNS server and, if not, request it and stop
    if (_IpProtocol == ETH_IPV6 && _DnsProtocol == DNS_PROTOCOL_UDNS)
    {
        if (!Ar6CheckHaveMacAndFetchIfNot(NdpDnsServer)) return DO_NOTHING;
    }
    
    NetTraceHostCheckIp6(DnsQueryIp6);
    
    if (DnsQueryTrace || NetTraceHostGetMatched()) logQuery();
    
    static uint16_t id = 0;
    DnsHdrId = ++id;
    DnsHdrIsReply          = false;
    DnsHdrIsAuthoritative  = false; //Added 12/12/2020
    DnsHdrIsRecursiveQuery = false;
    
    DnsHdrQdcount = 1;
    DnsHdrAncount = 0;
    DnsHdrNscount = 0;
    DnsHdrArcount = 0;    

    DnsHdrWrite();
    char* p = DnsHdrData;
    
    if      (DnsQueryIp4    ) DnsNameEncodeIp4(DnsQueryIp4,  &p);
    else if (DnsQueryIp6[0] ) DnsNameEncodeIp6(DnsQueryIp6,  &p);
    else if (DnsQueryName[0]) DnsNameEncodePtr(DnsQueryName, &p);
    else return DO_NOTHING;
    
    *p++ = 0;
    *p++ = _RecordType;
    *p++ = _DnsProtocol == DNS_PROTOCOL_MDNS && MDNS_UNICAST ? 0x80 : 0; //Set the 15th bit (UNICAST_RESPONSE) to 1 if MDNS
    *p++ = 1;  //QCLASS_IN = 1 - internet
    
    *pSize = p - DnsHdrPacket;
    
    DnsQueryIsBusy = false;
    
    if (DnsQueryTrace || NetTraceHostGetMatched()) DnsHdrLog(_DnsProtocol);

    int dest = DO_NOTHING;

    switch (_DnsProtocol)
    {
        case DNS_PROTOCOL_UDNS:  dest =   UNICAST_DNS;   break;   //IPv6 ==> NdpDnsServer; IPv4 ==> DhcpDnsServer
        case DNS_PROTOCOL_MDNS:  dest = MULTICAST_MDNS;  break;
        case DNS_PROTOCOL_LLMNR: dest = MULTICAST_LLMNR; break;
        default:
            LogTimeF("DNS unknown query protocol %d\r\n", _DnsProtocol);
            return DO_NOTHING;
    }
    
    return ActionMakeFromDestAndTrace(dest, DnsQueryTrace || NetTraceHostGetMatched());
}
