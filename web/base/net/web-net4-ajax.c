#include   <stdio.h>

#include "web/http/http.h"
#include "net/resolve/ar4.h"
#include "net/resolve/nr.h"
#include "net/udp/dhcp/dhcp.h"

void WebNet4Ajax()
{
    HttpOk("text/plain; charset=UTF-8", "no-cache", NULL, NULL);
    
    HttpAddInt32AsHex(DhcpLocalIp);          HttpAddChar('\n');
    HttpAddText      (DhcpDomainName);       HttpAddChar('\n');
    HttpAddText      (DhcpHostName);         HttpAddChar('\n');
    HttpAddInt32AsHex(DhcpNtpIp);            HttpAddChar('\n');
    HttpAddInt32AsHex(DhcpDnsServerIp);      HttpAddChar('\n');
    HttpAddInt32AsHex(DhcpServerIp);         HttpAddChar('\n');
    HttpAddInt32AsHex(DhcpRouterIp);         HttpAddChar('\n');
    HttpAddInt32AsHex(DhcpSubnetMask);       HttpAddChar('\n');
    HttpAddInt32AsHex(DhcpBroadcastIp);      HttpAddChar('\n');
    HttpAddInt32AsHex(DhcpLeaseTime);        HttpAddChar('\n');
    HttpAddInt32AsHex(DhcpRenewalT1);        HttpAddChar('\n');
    HttpAddInt32AsHex(DhcpRenewalT2);        HttpAddChar('\n');
    HttpAddInt32AsHex(DhcpGetElapsedLife()); HttpAddChar('\n');
    HttpAddChar('\f');
    
    Ar4SendAjax();
}
