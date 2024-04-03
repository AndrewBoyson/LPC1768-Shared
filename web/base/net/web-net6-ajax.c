#include   <stdio.h>

#include "web/http/http.h"
#include "net/ip6/icmp/ndp/ndp.h"
#include "net/ip6/slaac.h"
#include "net/resolve/ar6.h"
#include "net/resolve/nr.h"

void WebNet6Ajax()
{
    HttpOk("text/plain; charset=UTF-8", "no-cache", NULL, NULL);
    
    char nibble;
    
    //Slaac address
    for (char* p = SlaacLinkLocalIp; p < SlaacLinkLocalIp + 16; p++) HttpAddByteAsHex(*p);         HttpAddChar('\n');
    
    //NDP
    HttpAddInt32AsHex(NdpHopLimit);                                                                HttpAddChar('\n');
    HttpAddInt32AsHex(NdpMtu);                                                                     HttpAddChar('\n');
    nibble = 0;
    if (NdpManagedConfiguration  ) nibble |= 1; //4
    if (NdpOtherConfiguration    ) nibble |= 2; //4
    HttpAddNibbleAsHex(nibble);                                                                    HttpAddChar('\n');
    for (char* p = NdpRouterMac; p < NdpRouterMac + 6; p++) HttpAddByteAsHex(*p);                  HttpAddChar('\n');
    HttpAddInt32AsHex(NdpGetLease());                                                              HttpAddChar('\n');
    HttpAddInt32AsHex(NdpGetElapsedLife());                                                        HttpAddChar('\n');
    
    //DNS address
    for (char* p = NdpDnsServer; p < NdpDnsServer + 16; p++) HttpAddByteAsHex(*p);                 HttpAddChar('\n');
    HttpAddInt32AsHex(NdpDnsLifetime);                                                             HttpAddChar('\n');
    
    //Unique local prefix
    nibble = 0;
    if (NdpUniqueLocalPrefixFlagL) nibble |= 1; //4
    if (NdpUniqueLocalPrefixFlagA) nibble |= 2; //4
    HttpAddNibbleAsHex(nibble);                                                                    HttpAddChar('\n');
    HttpAddInt32AsHex(NdpUniqueLocalPrefixLength);                                                 HttpAddChar('\n');
    for (char* p = NdpUniqueLocalPrefix; p < NdpUniqueLocalPrefix + 16; p++) HttpAddByteAsHex(*p); HttpAddChar('\n');
    HttpAddInt32AsHex(NdpUniqueLocalPrefixValidLifetime);                                          HttpAddChar('\n');
    HttpAddInt32AsHex(NdpUniqueLocalPrefixPreferredLifetime);                                      HttpAddChar('\n');
    
    //Global prefix
    nibble = 0;
    if (NdpGlobalPrefixFlagL     ) nibble |= 1; //4
    if (NdpGlobalPrefixFlagA     ) nibble |= 2; //4
    HttpAddNibbleAsHex(nibble);                                                                    HttpAddChar('\n');
    HttpAddInt32AsHex(NdpGlobalPrefixLength);                                                      HttpAddChar('\n');
    for (char* p = NdpGlobalPrefix; p < NdpGlobalPrefix + 16; p++) HttpAddByteAsHex(*p);           HttpAddChar('\n');
    HttpAddInt32AsHex(NdpGlobalPrefixValidLifetime);                                               HttpAddChar('\n');
    HttpAddInt32AsHex(NdpGlobalPrefixPreferredLifetime);                                           HttpAddChar('\n');
    
    
    HttpAddChar('\f');
    
    Ar6SendAjax();
}
