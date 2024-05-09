#include <stdint.h>
#include <stdbool.h>

#include "net.h"
#include "link/link.h"
#include "tcp/tcp.h"
#include "udp/dhcp/dhcp.h"
#include "udp/dns/dns.h"
#include "resolve/ar4.h"
#include "resolve/ar6.h"
#include "resolve/nr.h"
#include "ip6/slaac.h"
#include "ip6/icmp/ndp/ndp.h"
#include "udp/ntp/ntp.h"
#include "udp/tftp/tftp.h"

#include "lpc1768/led.h"
#include "lpc1768/reset/restart.h"

char* NetName = 0; //Pointer assigned during init from the application so no memory allocated here

bool NetTraceStack      = false;
bool NetTraceNewLine    = false;
bool NetTraceVerbose    = false;

static bool hostMatched = false;
bool NetTraceHostGetMatched()
{
    return hostMatched;
}
void NetTraceHostResetMatched()
{
    hostMatched = false;
}
char NetTraceHost[2];
void NetTraceHostCheckIp6(char* ip)
{
    if (!ip[0]) return;
    if (NetTraceHost[0] != ip[14]) return;
    if (NetTraceHost[1] != ip[15]) return;
    hostMatched = true;
}
void NetTraceHostCheckMac(char* mac)
{
    if (NetTraceHost[0] != mac[4]) return;
    if (NetTraceHost[1] != mac[5]) return;
    hostMatched = true;
}
int16_t NetToHost16(int16_t n)
{
    int16_t h;
    
    char* pn = (char*)&n;
    char* ph = (char*)&h + 1;
    
    *ph = *pn; ph--; pn++; // 1<-0
    *ph = *pn;             // 0<-1

    return h;
}
int32_t NetToHost32(int32_t n)
{
    int32_t h;
    
    char* pn = (char*)&n;
    char* ph = (char*)&h + 3;
    
    *ph = *pn; ph--; pn++; // 3<-0
    *ph = *pn; ph--; pn++; // 2<-1
    *ph = *pn; ph--; pn++; // 1<-2
    *ph = *pn;             // 0<-3

    return h;
}
int64_t NetToHost64(int64_t n)
{
    int64_t h;
    
    char* pn = (char*)&n;
    char* ph = (char*)&h + 7;
    
    *ph = *pn; ph--; pn++; // 7<-0
    *ph = *pn; ph--; pn++; // 6<-1
    *ph = *pn; ph--; pn++; // 5<-2
    *ph = *pn; ph--; pn++; // 4<-3
    *ph = *pn; ph--; pn++; // 3<-4
    *ph = *pn; ph--; pn++; // 2<-5
    *ph = *pn; ph--; pn++; // 1<-6
    *ph = *pn;             // 0<-7

    return h;
}
void NetInvert16(void* h, void* n)
{
    char* pn = (char*)n;
    char* ph = (char*)h + 1;
    
    *ph = *pn; ph--; pn++; // 1<-0
    *ph = *pn;             // 0<-1
}
void NetInvert32(void* h, void* n)
{    
    char* pn = (char*)n;
    char* ph = (char*)h + 3;
    
    *ph = *pn; ph--; pn++; // 3<-0
    *ph = *pn; ph--; pn++; // 2<-1
    *ph = *pn; ph--; pn++; // 1<-2
    *ph = *pn;             // 0<-3
}
void NetInvert64(void* h, void* n)
{   
    char* pn = (char*)n;
    char* ph = (char*)h + 7;
    
    *ph = *pn; ph--; pn++; // 7<-0
    *ph = *pn; ph--; pn++; // 6<-1
    *ph = *pn; ph--; pn++; // 5<-2
    *ph = *pn; ph--; pn++; // 4<-3
    *ph = *pn; ph--; pn++; // 3<-4
    *ph = *pn; ph--; pn++; // 2<-5
    *ph = *pn; ph--; pn++; // 1<-6
    *ph = *pn;             // 0<-7
}

void NetDirect16(void* h, void* n)
{
    char* pn = (char*)n;
    char* ph = (char*)h;
    
    *ph = *pn; ph++; pn++; // 0<-0
    *ph = *pn;             // 1<-1
}
void NetDirect32(void* h, void* n)
{    
    char* pn = (char*)n;
    char* ph = (char*)h;
    
    *ph = *pn; ph++; pn++; // 0<-0
    *ph = *pn; ph++; pn++; // 1<-1
    *ph = *pn; ph++; pn++; // 2<-2
    *ph = *pn;             // 3<-3
}
void NetDirect64(void* h, void* n)
{   
    char* pn = (char*)n;
    char* ph = (char*)h;
    
    *ph = *pn; ph++; pn++; // 0<-0
    *ph = *pn; ph++; pn++; // 1<-1
    *ph = *pn; ph++; pn++; // 2<-2
    *ph = *pn; ph++; pn++; // 3<-3
    *ph = *pn; ph++; pn++; // 4<-4
    *ph = *pn; ph++; pn++; // 5<-5
    *ph = *pn; ph++; pn++; // 6<-6
    *ph = *pn;             // 7<-7
}
void NetInit(char* name, char* linkLedPin, char* speedLedPin)
{
	NetName = name;
        LinkInit(linkLedPin, speedLedPin);
         TcpInit();
         Ar4Init();
         Ar6Init();
          NrInit();
       SlaacInit();
         NtpInit();
}
void NetMain()
{
     LinkMain();
      TcpMain();
      Ar4Main();
      Ar6Main();
       NrMain();
      DnsMain();
}