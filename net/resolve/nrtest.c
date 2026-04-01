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
    
    if (!NrTest[0]) return;

	_addr4 = Ip4AddrParse(NrTest);
	Ip6AddrParse(NrTest, _addr6);
	if      (_addr4                 ) //Have an ip4 address
	{
		_makeRequestForNameFromAddr4 = true;
	}
	else if (!Ip6AddrIsEmpty(_addr6)) //Have an ip6 address
	{
		_makeRequestForNameFromAddr6 = true;
	}
	else                              //Have a name
	{
		_makeRequestForAddr4FromName = true;
		_makeRequestForAddr6FromName = true;
	}
}

void NrTestMain(void)
{
    if (DnsQueryIsBusy) return;
    
    
    if (_makeRequestForNameFromAddr4)
    {
        LogTime("NrTest - making "); DnsProtocolLog(_dnsProtocol); Log(" request over "); EthProtocolLog(_ipProtocol); LogF(" for name from '%s'\r\n", NrTest);
        DnsQueryNameFromIp4(_addr4, _dnsProtocol, _ipProtocol);
        _makeRequestForNameFromAddr4 = false;
        return;
    }
    if (_makeRequestForNameFromAddr6)
    {
        LogTime("NrTest - making "); DnsProtocolLog(_dnsProtocol); Log(" request over "); EthProtocolLog(_ipProtocol); LogF(" for name from '%s'\r\n", NrTest);
        DnsQueryNameFromIp6(_addr6, _dnsProtocol, _ipProtocol);
        _makeRequestForNameFromAddr6 = false;
        return;
    }
    if (_makeRequestForAddr4FromName)
    {
        LogTime("NrTest - making "); DnsProtocolLog(_dnsProtocol); Log(" request over "); EthProtocolLog(_ipProtocol); LogF(" for A from '%s'\r\n", NrTest);
        DnsQueryIp4FromName(NrTest, _dnsProtocol, _ipProtocol);
        _makeRequestForAddr4FromName = false;
        return;
    }
    if (_makeRequestForAddr6FromName)
    {
        LogTime("NrTest - making "); DnsProtocolLog(_dnsProtocol); Log(" request over "); EthProtocolLog(_ipProtocol); LogF(" for AAAA from '%s'\r\n", NrTest);
        DnsQueryIp6FromName(NrTest, _dnsProtocol, _ipProtocol);
        _makeRequestForAddr6FromName = false;
        return;
    }
}