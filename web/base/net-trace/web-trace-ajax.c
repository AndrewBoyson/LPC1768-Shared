#include  <stdint.h>

#include "web/http/http.h"
#include "log/log.h"
#include "net/net.h"
#include "net/link/link.h"
#include "net/udp/dns/dns.h"
#include "net/udp/dns/dnsname.h"
#include "net/udp/dns/dnsquery.h"
#include "net/udp/dns/dnsreply.h"
#include "net/udp/dns/dnsserver.h"
#include "net/udp/ntp/ntp.h"
#include "net/udp/ntp/ntpclient.h"
#include "net/udp/dhcp/dhcp.h"
#include "net/udp/tftp/tftp.h"
#include "net/ip6/icmp/ndp/ns.h"
#include "net/resolve/nr.h"
#include "net/ip4/icmp/echo4.h"
#include "net/ip6/icmp/echo6.h"
#include "net/ip6/icmp/dest6.h"
#include "net/ip6/icmp/ndp/ra.h"
#include "net/ip6/icmp/ndp/rs.h"
#include "net/resolve/ar4.h"
#include "net/resolve/ar6.h"
#include "net/arp/arp.h"
#include "net/ip4/ip4.h"
#include "net/ip6/ip6.h"
#include "net/udp/udp.h"
#include "net/tcp/tcp.h"
#include "web/web.h"

void WebTraceAjax()
{
    HttpOk("text/plain; charset=UTF-8", "no-cache", NULL, NULL);
    char nibble;
    
    nibble = 0; //0
    if ( DnsSendRequestsViaIp4) nibble |= 2;
    if ( NtpClientQuerySendRequestsViaIp4) nibble |= 4;
    if (TftpSendRequestsViaIp4) nibble |= 8;
    HttpAddNibbleAsHex(nibble);
    
    HttpAddByteAsHex(NetTraceHost[0]);  //1, 2
    HttpAddByteAsHex(NetTraceHost[1]);  //3, 4
    
    nibble = 0; //5
    if (NetTraceStack   ) nibble |= 1;
    if (NetTraceNewLine ) nibble |= 2;
    if (NetTraceVerbose ) nibble |= 4;
    if (LinkTrace       ) nibble |= 8;
    HttpAddNibbleAsHex(nibble);
    
    nibble = 0; //6
    if (DnsNameTrace    ) nibble |= 1;
    if (DnsQueryTrace   ) nibble |= 2;
    if (DnsReplyTrace   ) nibble |= 4;
    if (DnsServerTrace  ) nibble |= 8;
    HttpAddNibbleAsHex(nibble);
    
    nibble = 0; //7
    if (NtpTrace        ) nibble |= 1;
    if (DhcpTrace       ) nibble |= 2;
    if (NsTraceRecvSol  ) nibble |= 4;
    if (NsTraceRecvAdv  ) nibble |= 8;
    HttpAddNibbleAsHex(nibble);
    
    nibble = 0; //8
    if (NsTraceSendSol  ) nibble |= 1;
    if (Nr4Trace        ) nibble |= 2;
    if (NrTrace         ) nibble |= 4;
    if (NtpClientTrace  ) nibble |= 8;
    HttpAddNibbleAsHex(nibble);
    
    nibble = 0; //9
    if (Echo4Trace      ) nibble |= 4;
    if (Echo6Trace      ) nibble |= 8;
    HttpAddNibbleAsHex(nibble);
    
    nibble = 0; //10
    if (Dest6Trace      ) nibble |= 1;
    if (RaTrace         ) nibble |= 2;
    if (RsTrace         ) nibble |= 4;
    if (Ar4Trace        ) nibble |= 8;
    HttpAddNibbleAsHex(nibble);
    
    nibble = 0; //11
    if (Ar6Trace        ) nibble |= 1;
    if (ArpTrace        ) nibble |= 2;
    if (Ip4Trace        ) nibble |= 4;
    if (Ip6Trace        ) nibble |= 8;
    HttpAddNibbleAsHex(nibble);
    
    nibble = 0; //12
    if (UdpTrace        ) nibble |= 1;
    if (TcpTrace        ) nibble |= 2;
    if (WebTrace        ) nibble |= 4;
    if (TftpTrace       ) nibble |= 8;
    HttpAddNibbleAsHex(nibble);
}

