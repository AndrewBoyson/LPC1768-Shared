#include <string.h>

#include "web/http/http.h"
#include "net/resolve/nr.h"
#include "net/resolve/nrtest.h"
#include "net/eth/eth.h"
#include "net/udp/dns/dns.h"

void WebNetQuery(char* pQuery)
{
    while (pQuery)
    {
        char* pName;
        char* pValue;
        pQuery = HttpQuerySplit(pQuery, &pName, &pValue);
        HttpQueryUnencode(pValue);
		
        //int value = HttpQueryValueAsInt(pValue);
        
        if (HttpSameStr(pName, "name-to-resolve"))
        {
            strncpy(NrTest, pValue, NR_NAME_MAX_LENGTH);
        }
        if (HttpSameStr(pName, "request-ipv6-mdns"))
        {
            NrTestSendRequest(ETH_IPV6, DNS_PROTOCOL_MDNS);
        }
        if (HttpSameStr(pName, "request-ipv6-llmnr"))
        {
            NrTestSendRequest(ETH_IPV6, DNS_PROTOCOL_LLMNR);
        }
        if (HttpSameStr(pName, "request-ipv6-udns"))
        {
            NrTestSendRequest(ETH_IPV6, DNS_PROTOCOL_UDNS);
        }
        if (HttpSameStr(pName, "request-ipv4-mdns"))
        {
            NrTestSendRequest(ETH_IPV4, DNS_PROTOCOL_MDNS);
        }
        if (HttpSameStr(pName, "request-ipv4-llmnr"))
        {
            NrTestSendRequest(ETH_IPV4, DNS_PROTOCOL_LLMNR);
        }
        if (HttpSameStr(pName, "request-ipv4-udns"))
        {
            NrTestSendRequest(ETH_IPV4, DNS_PROTOCOL_UDNS);
        }
    }
}

