#include <stdbool.h>

#include "net/eth/eth.h"
#include "ar4.h"
#include "ar6.h"
#include "net/eth/mac.h"
#include "nr.h"
#include "net/ip6/ip6addr.h"

static bool resolve4(char* server, uint32_t* pip)
{
    //Check if have IP, if not, then request it and stop
    NrNameToAddress4(server, pip);
    if (!*pip)
    {
        NrMakeRequestForAddress4FromName(server); //The request is only repeated if made after a freeze time - call as often as you want.
        return false;
    }

    //Check if have MAC and, if not, request it and stop
    char mac[6];
    Ar4IpToMac(*pip, mac);
    if (MacIsEmpty(mac))
    {
        Ar4MakeRequestForMacFromIp(*pip); //The request is only repeated if made after a freeze time - call as often as you want.
        return false;
    }
    
    return true;
}
static bool resolve6(char* server, char* pip)
{
    //Check if have IP, if not, then request it and stop
    NrNameToAddress6(server, pip);
    if (Ip6AddrIsEmpty(pip))
    {
        NrMakeRequestForAddress6FromName(server); //The request is only repeated if made after a freeze time - call as often as you want.
        return false;
    }

    //Check if have MAC and, if not, request it and stop
    char mac[6];
    Ar6IpToMac(pip, mac);
    if (MacIsEmpty(mac))
    {
        Ar6MakeRequestForMacFromIp(pip); //The request is only repeated if made after a freeze time - call as often as you want.
        return false;
    }
    
    return true;
}
bool Resolve(char* remoteHost, int type, uint32_t* pIp4Address, char* pIp6Address)
{
    if (type == ETH_IPV4) return resolve4(remoteHost, pIp4Address);
    if (type == ETH_IPV6) return resolve6(remoteHost, pIp6Address);
    return false;
}
