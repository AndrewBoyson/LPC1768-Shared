#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "log/log.h"
#include "lpc1768/mstimer/mstimer.h"
#include "net/net.h"
#include "net/eth/eth.h"
#include "net/eth/mac.h"
#include "net/udp/dhcp/dhcp.h"
#include "net/udp/dns/dns.h"
#include "net/udp/dns/dnsquery.h"
#include "net/udp/dns/dnslabel.h"
#include "web/http/http.h"
#include "net/ip6/ip6addr.h"
#include "net/ip4/ip4addr.h"
#include "net/resolve/nr.h"
#include "net/resolve/nrtest.h"
#include "net/ip6/icmp/ndp/ndp.h"
#include "net/resolve/ar6.h"

bool Nr4Trace = false; //Do not use
bool NrTrace = false;

#define CACHE_TIMEOUT_MS 3600 * 1000
#define STALE_TIMEOUT_MS 1800 * 1000
#define EMPTY_TIMEOUT_MS  300 * 1000
#define REPLY_TIMEOUT_MS         100

#define RECORDS_COUNT 50

#define STATE_EMPTY        0
#define STATE_WANT         1
#define STATE_WAIT_FOR_ETH 2
#define STATE_WAIT_TIMEOUT 3
#define STATE_VALID        4

#define TODO_NONE              0
#define TODO_NAME_FROM_ADDRESS 1
#define TODO_ADDRESS_FROM_NAME 2

#define ADDR_TYPE_A    4
#define ADDR_TYPE_AAAA 6

struct record
{
    uint32_t replyMs; //Need this in addition to the ageMs for when refreshing an existing entry
    uint32_t ageMs;
    union
    {
        uint32_t A;
        uint8_t  AAAA[16];
        char     address[16];
    };
    uint8_t  addrType; //ADDR_TYPE_A (4) or ADDR_TYPE_AAAA (6)
    uint8_t  todo;
    uint8_t  state;
    uint8_t  dnsProtocol;
    uint16_t  ipProtocol;
    char     name[NR_NAME_MAX_LENGTH];
};
static struct record records[RECORDS_COUNT];

static void addrClear(const uint8_t addrType, char* ip)
{
    switch (addrType)
    {
        case ADDR_TYPE_A   : *(uint32_t*)ip = 0; break;
        case ADDR_TYPE_AAAA: Ip6AddrClear(ip);   break;
        default:             LogTimeF("NR - addrClear - Unknown addrType %d\r\n", addrType); break;
    }
}
static bool addrIsEmpty(const uint8_t addrType, const char* ip)
{
    switch (addrType)
    {
        case ADDR_TYPE_A   : return *(uint32_t*)ip == 0;
        case ADDR_TYPE_AAAA: return Ip6AddrIsEmpty(ip);
        default:             return LogTimeF("NR - addrIsEmpty - Unknown addrType %d\r\n", addrType);
    }
}
static int addrLog(uint8_t addrType, const char* ip)
{
    switch (addrType)
    {
        case ADDR_TYPE_A   : return Ip4AddrLog(*(uint32_t*)ip);
        case ADDR_TYPE_AAAA: return Ip6AddrLog(ip);
        default:             return LogTimeF("NR - addrLog - Unknown addrType %d\r\n", addrType);
    }
}

static bool addrIsSame(uint8_t addrType, const char* ipA, const char* ipB)
{
    switch (addrType)
    {
        case ADDR_TYPE_A   : return *(uint32_t*)ipA == *(uint32_t*)ipB;
        case ADDR_TYPE_AAAA: return Ip6AddrIsSame(ipA, ipB);
        default:             return LogTimeF("NR - addrIsSame - Unknown addrType %d\r\n", addrType);
    }
}

static void addrCopy(uint8_t addrType, char* ipTo, const char* ipFrom)
{
    switch (addrType)
    {
        case ADDR_TYPE_A   : *(uint32_t*)ipTo = *(uint32_t*)ipFrom; break;
        case ADDR_TYPE_AAAA: Ip6AddrCopy(ipTo, ipFrom);             break;
        default:             LogTimeF("NR - addrCopy - Unknown addrType %d\r\n", addrType); break;
    }
}

static int getExistingAddress(char* address, uint8_t addrType)
{
    for (int i = 0; i < RECORDS_COUNT; i++)
    {
        if (records[i].state == STATE_EMPTY) continue;
        if (records[i].addrType != addrType) continue;
        if (addrIsSame(addrType, records[i].address, address)) return i;
    }
    return -1;
}
static int getExistingName(char* name, uint8_t addrType)
{
    for (int i = 0; i < RECORDS_COUNT; i++)
    {
        if (records[i].state == STATE_EMPTY) continue;
        if (records[i].addrType != addrType) continue;
        if (DnsLabelIsSame(records[i].name, name)) return i;
    }
    return -1;
}
static int getOldest()
{
    int        iOldest = 0;
    uint32_t ageOldest = 0;
    for (int i = 0; i < RECORDS_COUNT; i++)
    {
        if (records[i].state == STATE_EMPTY) return i; //Found an empty slot so just return it
        uint32_t age = MsTimerCount - records[i].ageMs;
        if (age >= ageOldest)
        {
            ageOldest = age;
              iOldest = i;
        }
    }  
    return iOldest;                            //Otherwise return the oldest
}
static void makeRequestForNameFromAddress(uint8_t addrType, void* address )
{
    //Don't treat non ips
    if (addrIsEmpty(addrType, address)) return;
    int i;
    
    //If a valid record already exists then request an update
    i = getExistingAddress(address, addrType);
    if (i > -1)
    {
        if (records[i].state != STATE_VALID) return;
        if (records[i].name[0] == 0)
        {
            if (!MsTimerRelative(records[i].ageMs, EMPTY_TIMEOUT_MS)) return;
        }
        else
        {
            if (!MsTimerRelative(records[i].ageMs, STALE_TIMEOUT_MS)) return;
        }
        if (NrTrace)
        {
            LogTimeF("NR - record %2d - renew name of ", i);
            addrLog(addrType, address);
            Log("\r\n");
        }
        //Leave the address as is
        //Leave the name as is
        //Leave age as is
    }
    else
    {
        //If a record does not exist then find the first empty slot and add the IP and date
        i = getOldest();
        if (NrTrace)
        {
            LogTimeF("NR - record %2d - request name of ", i);
            addrLog(addrType, address);
            Log("\r\n");
        }
        addrCopy(addrType, records[i].address, address);  //Set the address
        records[i].name[0]  = 0;                          //Clear the name
        records[i].ageMs    = MsTimerCount;               //Start age
    }
    records[i].todo        = TODO_NAME_FROM_ADDRESS;
    records[i].addrType    = addrType;
    records[i].state       = STATE_WANT;
    records[i].replyMs     = MsTimerCount;
    records[i].ipProtocol  = ETH_NONE;
    records[i].dnsProtocol = DNS_PROTOCOL_NONE;
}
static void makeRequestForAddressFromName(char* name, uint8_t addrType)
{
    //Don't treat non names
    if (!name[0]) return;
    int i;
        
    //If a valid record already exists then request an update
    i = getExistingName(name, addrType);
    if (i > -1)
    {
        if (records[i].state != STATE_VALID) return;
        if (addrIsEmpty(records[i].addrType, records[i].address))
        {
            if (!MsTimerRelative(records[i].ageMs, EMPTY_TIMEOUT_MS)) return;
        }
        else
        {
            if (!MsTimerRelative(records[i].ageMs, STALE_TIMEOUT_MS)) return;
        }
        if (NrTrace)
        {
            if (addrType == ADDR_TYPE_A) LogTimeF("NR - record %2d - renew A of %s\r\n", i, name);
            else                         LogTimeF("NR - record %2d - renew AAAA of %s\r\n", i, name);
        }
        //Leave name as is
        //Leave the address as is
        //Leave age as is
    }
    else
    {   
        //If a record does not exist then find the first empty slot and add the name and date
        i = getOldest();
        if (NrTrace)
        {
            if (addrType == ADDR_TYPE_A) LogTimeF("NR - record %2d - request A of %s\r\n", i, name);
            else                         LogTimeF("NR - record %2d - request AAAA of %s\r\n", i, name);
        }
        strncpy(records[i].name, name, NR_NAME_MAX_LENGTH); //Set the name
        records[i].name[NR_NAME_MAX_LENGTH - 1] = 0;
        addrClear(addrType, records[i].address);            //Clear the address
        records[i].ageMs  = MsTimerCount;                   //Start age
    }
    records[i].todo        = TODO_ADDRESS_FROM_NAME;
    records[i].addrType    = addrType;
    records[i].state       = STATE_WANT;
    records[i].replyMs     = MsTimerCount;
    records[i].ipProtocol  = ETH_NONE;
    records[i].dnsProtocol = DNS_PROTOCOL_NONE;
}
static void addEntry(uint8_t addrType, void* address, char* name, int dnsProtocol, int ipProtocol)
{

    //Ignore records which do not have both address and name    
    if (addrIsEmpty(addrType, address) || name == 0 || name[0] == 0)
    {
        if (NrTrace) LogTimeF("NR - ignoring invalid entry\r\n");
        return;
    }
    //Ignore records with the name 'UNKNOWN'
    if (strcmp(name, "UNKNOWN") == 0) return;
    
    //Delete any existing records linked to the new entry
    for (int i = 0; i < RECORDS_COUNT; i++)
    {
        if (records[i].state == STATE_EMPTY) continue;
        if (records[i].addrType != addrType) continue;
        bool sameAddress = addrIsSame(addrType, records[i].address, address);
        bool sameName = DnsLabelIsSame(records[i].name, name);
        bool noAddress = addrIsEmpty(addrType, records[i].address);
        bool requesting = records[i].state != STATE_VALID;
        if (sameAddress || (sameName && (noAddress || requesting))) records[i].state = STATE_EMPTY;
    }

    //Add the new entry
    int i = getOldest();
    if (NrTrace)
    {
        LogTimeF("NR - record %2d - received ", i);
        EthProtocolLog(ipProtocol);
        Log(" ");
        DnsProtocolLog(dnsProtocol);
        Log(" ");
        addrLog(addrType, address);
        Log(" == '");
        Log(name);
        Log("'\r\n");
    }
    records[i].todo        = TODO_NONE;
    records[i].ageMs       = MsTimerCount;
    records[i].addrType    = addrType;
    addrCopy(addrType, records[i].address, address);
    records[i].dnsProtocol = dnsProtocol;
    records[i].ipProtocol  = ipProtocol;
    records[i].state       = STATE_VALID;
    strncpy(records[i].name, name, NR_NAME_MAX_LENGTH);
    records[i].name[NR_NAME_MAX_LENGTH - 1] = 0;
}
static void addressToName(uint8_t addrType, void* address, char* name)
{
    for (int i = 0; i < RECORDS_COUNT; i++)
    {
        if (records[i].state == STATE_EMPTY) continue;
        if (addrIsSame(addrType, records[i].address, address))
        {
            strcpy(name, records[i].name);
            return;
        }
    }
    name[0] = 0;
}
static void nameToAddress(char* name, uint8_t addrType, void* address)
{
    uint32_t newest = 0xFFFFFFFF;
    addrClear(addrType, address);
    for (int i = 0; i < RECORDS_COUNT; i++)
    {
        if (records[i].state == STATE_EMPTY) continue;
        if (records[i].addrType != addrType) continue;
        if(addrIsEmpty(records[i].addrType, records[i].address)) continue;
        if (!DnsLabelIsSame(records[i].name, name)) continue;
        uint32_t age = MsTimerCount - records[i].ageMs;
        if (age <= newest)
        {
            newest = age;
            addrCopy(addrType, address, records[i].address);
        }
    }
}
void NrMakeRequestForNameFromAddress6(char*   pAddress) { makeRequestForNameFromAddress(ADDR_TYPE_AAAA, pAddress); }
void NrMakeRequestForNameFromAddress4(uint32_t address) { makeRequestForNameFromAddress(ADDR_TYPE_A,    &address); }
void NrMakeRequestForAddress6FromName(char* name)       { makeRequestForAddressFromName(name, ADDR_TYPE_AAAA); }
void NrMakeRequestForAddress4FromName(char* name)       { makeRequestForAddressFromName(name, ADDR_TYPE_A   ); }
void NrAddAddress6(char*   pAddress, char* name, int dnsProtocol) { addEntry(ADDR_TYPE_AAAA, pAddress, name, dnsProtocol, EthProtocol); }
void NrAddAddress4(uint32_t address, char* name, int dnsProtocol) { addEntry(ADDR_TYPE_A,    &address, name, dnsProtocol, EthProtocol); }
void NrNameToAddress6(char* name,      char* address) { nameToAddress(name, ADDR_TYPE_AAAA, address); }
void NrNameToAddress4(char* name, uint32_t* pAddress) { nameToAddress(name, ADDR_TYPE_A,   pAddress); }
void NrAddress6ToName(char*   pAddress, char* name)   { addressToName(ADDR_TYPE_AAAA, pAddress, name); }
void NrAddress4ToName(uint32_t address, char* name)   { addressToName(ADDR_TYPE_A,    &address, name); }

static char letterFromStateAndProtocol(uint8_t dnsState, uint8_t dnsProtocol, uint16_t ipProtocol)
{
    switch (dnsState)
    {
        case STATE_WANT:         return '}';
        case STATE_WAIT_FOR_ETH: return ']';
        case STATE_WAIT_TIMEOUT: return '>';
        
        case STATE_VALID:
            switch (dnsProtocol)
            {
                case DNS_PROTOCOL_UDNS:  if (ipProtocol == ETH_IPV4) return 'd' ; else return 'D';
                case DNS_PROTOCOL_MDNS:  if (ipProtocol == ETH_IPV4) return 'm' ; else return 'M';
                case DNS_PROTOCOL_LLMNR: if (ipProtocol == ETH_IPV4) return 'l' ; else return 'L';
                case DNS_PROTOCOL_NONE:  return '-';
                default:                 return '?';
            }
        default:                         return '~';
    }
}
void NrSendAjax()
{
    for (int i = 0; i < RECORDS_COUNT; i++)
    {
        if (records[i].state == STATE_EMPTY) continue;
        if (!addrIsEmpty(records[i].addrType, records[i].address) || records[i].name[0])
        {
            HttpAddByteAsHex(i);
            HttpAddChar('\t');
            HttpAddInt32AsHex(MsTimerCount - records[i].ageMs);
            HttpAddChar('\t');
            HttpAddNibbleAsHex(records[i].addrType);
            HttpAddChar('\t');
            switch (records[i].addrType)
            {
                case ADDR_TYPE_A:
                {
                    HttpAddInt32AsHex(records[i].A);
                    break;
                }
                case ADDR_TYPE_AAAA:
                    for (int b = 0; b < 16; b++) HttpAddByteAsHex(records[i].AAAA[b]);
                    break;
            }
            HttpAddChar('\t');
            HttpAddChar(letterFromStateAndProtocol(records[i].state, records[i].dnsProtocol, records[i].ipProtocol));
            HttpAddChar('\t');
            HttpAddText(records[i].name);
            HttpAddChar('\n');
        }
    }
}
static void clearCache(struct record* pr)
{
    if (MsTimerRelative(pr->ageMs, CACHE_TIMEOUT_MS)) pr->state = STATE_EMPTY;
}
static void queryNameFromIp(struct record* pr)
{
    if (addrIsEmpty(pr->addrType, pr->address))
    {
        LogTimeF("NR - record %2d - queryNameFromIp has no address\r\n", pr - records);
        return;
    }
    if (NrTrace)
    {
        LogTimeF("NR - record %2d - send ", pr - records);
        EthProtocolLog(pr->ipProtocol);
        Log(" ");
        DnsProtocolLog(pr->dnsProtocol);
        Log(" request for name of ");
        addrLog(pr->addrType, pr->address);
        Log("\r\n");
    }
    if (pr->addrType == ADDR_TYPE_A) 
    {
        //uint32_t address4 = addrToIp4(pr->address);
        DnsQueryNameFromIp4(pr->A, pr->dnsProtocol, pr->ipProtocol);
    }
    else
    {
        DnsQueryNameFromIp6(pr->address, pr->dnsProtocol, pr->ipProtocol);
    }
}
static void queryIpFromName(struct record* pr)
{
    if (NrTrace)
    {
        LogTimeF("NR - record %2d - send ", pr - records);
        EthProtocolLog(pr->ipProtocol);
        Log(" ");
        DnsProtocolLog(pr->dnsProtocol);
        if (pr->addrType == ADDR_TYPE_A) Log(" request for A of name '");
        else                             Log(" request for AAAA of name '");
        Log(pr->name);
        Log("'\r\n");
    }
    if (pr->addrType == ADDR_TYPE_A) DnsQueryIp4FromName(pr->name, pr->dnsProtocol, pr->ipProtocol);
    else                             DnsQueryIp6FromName(pr->name, pr->dnsProtocol, pr->ipProtocol);
}
static bool getIsExternal(struct record* pr)
{
    switch(pr->todo)
    {
        case TODO_NAME_FROM_ADDRESS:
            
            if (pr->addrType == ADDR_TYPE_AAAA)
            {
                return Ip6AddrIsExternal((char*)pr->AAAA);
            }
            else
            {
                return Ip4AddrIsExternal(pr->A);
            }
        case TODO_ADDRESS_FROM_NAME:
            return DnsLabelIsExternal(pr->name);
        default:
            LogTimeF("NR - getIsExternal - undefined todo '%d'\r\n", pr->todo);
            return false;
    }
}
static bool protocolIsAvailable(struct record* pr)
{
    bool isExternal = getIsExternal(pr);
    switch(pr->dnsProtocol)
    {
        case DNS_PROTOCOL_MDNS: return !isExternal;
        case DNS_PROTOCOL_LLMNR: return !isExternal;
        case DNS_PROTOCOL_UDNS:
            if (pr->ipProtocol == ETH_IPV6)
            {
                if (Ip6AddrIsEmpty(NdpDnsServer)) return false; //No DNS server so not ok
                if (isExternal) return true; //External and have DNS server so ok
                return false; //Internal but have no DHCP6 domain name so not ok
            }
            else //ETH_IPV4
            {
                if (DhcpDnsServerIp == 0) return false; //No DNS server so not ok
                if (isExternal) return true; //External and have DNS server so ok
                return DhcpDomainName[0] != 0; //Internal and have domain name so ok
            }
        case DNS_PROTOCOL_NONE: return true; //No protocol is valid as it designates 'not found'
        default:                return false;
    }
}
static void makeNextProtocol(struct record* pr)
{
    //If current protocol is empty or unknown then return with the first.
    switch (pr->ipProtocol)
    {
        case ETH_IPV6: break;
        case ETH_IPV4: break;
        default:
            pr->ipProtocol  = ETH_IPV6;
            pr->dnsProtocol = DNS_PROTOCOL_MDNS;
            return;
    }
    switch (pr->dnsProtocol)
    {
        case DNS_PROTOCOL_MDNS:  break;
        case DNS_PROTOCOL_LLMNR: break;
        case DNS_PROTOCOL_UDNS:  break;
        default:
            pr->ipProtocol  = ETH_IPV6;
            pr->dnsProtocol = DNS_PROTOCOL_MDNS;
            return;

    }
    switch(pr->dnsProtocol)
    {
        case DNS_PROTOCOL_MDNS: 
            if (pr->ipProtocol == ETH_IPV6)
            {
                pr->ipProtocol = ETH_IPV4;
            }
            else
            {
                pr->ipProtocol = ETH_IPV6;
                pr->dnsProtocol = DNS_PROTOCOL_LLMNR;
            }
            break;
        case DNS_PROTOCOL_LLMNR:
            if (pr->ipProtocol == ETH_IPV6)
            {
                pr->ipProtocol = ETH_IPV4;
            }
            else
            {
                pr->ipProtocol = ETH_IPV6;
                pr->dnsProtocol = DNS_PROTOCOL_UDNS;
            }
            break;
        case DNS_PROTOCOL_UDNS:
            if (pr->ipProtocol == ETH_IPV6)
            {
                pr->ipProtocol = ETH_IPV4;
            }
            else
            {
                pr->ipProtocol  = ETH_NONE;
                pr->dnsProtocol = DNS_PROTOCOL_NONE;
            }
            break;
    }
}

static void sendRequest(struct record* pr)
{
    //if (DnsQueryIsBusy) return;
    
    switch (pr->state)
    {
        case STATE_WANT:
            makeNextProtocol(pr);
            while (!protocolIsAvailable(pr)) makeNextProtocol(pr);
            if (!pr->dnsProtocol || !pr->ipProtocol) //No more protocols to try so resolution has failed
            {
                if (pr->todo == TODO_NAME_FROM_ADDRESS)
                {
                    if (NrTrace)
                    {
                        LogTimeF("NR - record %2d - request for name of ", pr - records);
                        addrLog(pr->addrType, pr->address);
                        Log(" has timed out\r\n");
                    }
                    pr->name[0] = 0;
                }
                if (pr->todo == TODO_ADDRESS_FROM_NAME)
                {
                    if (NrTrace)
                    {
                        LogTimeF("NR - record %2d - request for address of '", pr - records);
                        Log(pr->name);
                        Log("' has timed out\r\n");
                        Log("\r\n");
                    }
                    addrClear(pr->addrType, pr->address);
                }
                pr->todo   = TODO_NONE;
                pr->state  = STATE_VALID;
                pr->ageMs  = MsTimerCount;
            }
            else
            {
                pr->state  = STATE_WAIT_FOR_ETH;
            }
            break;
        case STATE_WAIT_FOR_ETH:
            if (!DnsQueryIsBusy)
            {   
                switch (pr->todo)
                {
                    case TODO_NAME_FROM_ADDRESS: queryNameFromIp(pr); break;
                    case TODO_ADDRESS_FROM_NAME: queryIpFromName(pr); break;
                }
                pr->state = STATE_WAIT_TIMEOUT;
                pr->replyMs = MsTimerCount;
            }
            break;
        case STATE_WAIT_TIMEOUT:
            if (MsTimerRelative(pr->replyMs, REPLY_TIMEOUT_MS))
            {
                pr->state = STATE_WANT;
            }
            break;
        default:
            break;
    }
}
void NrMain()
{
    static int i = -1;
    i++;
    if (i >= RECORDS_COUNT) i = 0;
    
    struct record* pr = &records[i];
    
    clearCache  (pr);
    sendRequest (pr);
    
    NrTestMain();
}
void NrInit()
{
    for (int i = 0; i < RECORDS_COUNT; i++) records[i].state = STATE_EMPTY;
}
