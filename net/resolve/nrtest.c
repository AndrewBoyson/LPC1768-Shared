#include <stdbool.h>
#include <stdint.h>

#include "nr.h"
#include "ar6.h"
#include "net/ip6/icmp/ndp/ndp.h"
#include "net/udp/dns/dnsquery.h"
#include "net/udp/dns/dns.h"
#include "net/eth/eth.h"
#include "net/ip4/ip4addr.h"
#include "net/ip6/ip6addr.h"
#include "log/log.h"
#include "net/eth/mac.h"

char NrTest[NR_NAME_MAX_LENGTH];

static int   _ipProtocol = 0;
static int  _dnsProtocol = 0;
static bool _makeRequestForNameFromAddr4 = false;
static bool _makeRequestForNameFromAddr6 = false;
static bool _makeRequestForAddr4FromName = false;
static bool _makeRequestForAddr6FromName = false;

static uint32_t _addr4;
static char     _addr6[16];

void NrTestSendRequest(int ipProtocol, int dnsProtocol)
{
    _ipProtocol  = ipProtocol;
    _dnsProtocol = dnsProtocol;
    
    if (NrTest[0])
    {
        _addr4 = Ip4AddrParse(NrTest);
        if (_addr4)
        {
            _makeRequestForNameFromAddr4 = true;
        }
        else
        {
            Ip6AddrParse(NrTest, _addr6);
            if (!Ip6AddrIsEmpty(_addr6))
            {
                _makeRequestForNameFromAddr6 = true;
            }
            else
            {
                _makeRequestForAddr4FromName = true;
                _makeRequestForAddr6FromName = true;
            }
        }
    }
}

/*
static bool getMacOfDnsServer6()
{
    //For IPv6 UDNS check if have the MAC for the DNS server and, if not, request it and stop
    if (_ipProtocol == ETH_IPV6 && _dnsProtocol == DNS_PROTOCOL_UDNS)
    {
        char mac[6];
        Ar6IpToMac(NdpDnsServer, mac);
        if (MacIsEmpty(mac))
        {
            Ar6MakeRequestForMacFromIp(NdpDnsServer); //The request is only repeated if made after a freeze time - call as often as you want.
            return false;
        }
    }
    return true;
}
*/

void NrTestMain(void)
{
    if (DnsQueryIsBusy) return;
    
    
    if (_makeRequestForNameFromAddr4)
    {
        //if (!getMacOfDnsServer6()) return;
        LogTime("NrTest - making "); DnsProtocolLog(_dnsProtocol); Log(" request over "); EthProtocolLog(_ipProtocol); LogF(" for name from '%s'\r\n", NrTest);
        DnsQueryNameFromIp4(_addr4, _dnsProtocol, _ipProtocol);
        _makeRequestForNameFromAddr4 = false;
        return;
    }
    if (_makeRequestForNameFromAddr6)
    {
        //if (!getMacOfDnsServer6()) return;
        LogTime("NrTest - making "); DnsProtocolLog(_dnsProtocol); Log(" request over "); EthProtocolLog(_ipProtocol); LogF(" for name from '%s'\r\n", NrTest);
        DnsQueryNameFromIp6(_addr6, _dnsProtocol, _ipProtocol);
        _makeRequestForNameFromAddr6 = false;
        return;
    }
    if (_makeRequestForAddr4FromName)
    {
        //if (!getMacOfDnsServer6()) return;
        LogTime("NrTest - making "); DnsProtocolLog(_dnsProtocol); Log(" request over "); EthProtocolLog(_ipProtocol); LogF(" for A from '%s'\r\n", NrTest);
        DnsQueryIp4FromName(NrTest, _dnsProtocol, _ipProtocol);
        _makeRequestForAddr4FromName = false;
        return;
    }
    if (_makeRequestForAddr6FromName)
    {
        //if (!getMacOfDnsServer6()) return;
        LogTime("NrTest - making "); DnsProtocolLog(_dnsProtocol); Log(" request over "); EthProtocolLog(_ipProtocol); LogF(" for AAAA from '%s'\r\n", NrTest);
        DnsQueryIp6FromName(NrTest, _dnsProtocol, _ipProtocol);
        _makeRequestForAddr6FromName = false;
        return;
    }
}