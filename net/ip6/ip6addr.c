#include <stdbool.h>
#include <string.h>

#include "log/log.h"
#include "ip6addr.h"
#include "web/http/http.h"
#include "net/action.h"
#include "net/ip6/icmp/ndp/ndp.h"
#include "net/udp/ntp/ntpclient.h"
#include "net/ip6/slaac.h"
#include "net/udp/tftp/tftp.h"

void Ip6AddrClear(char* ip)
{
    ip[ 0] = 0; //Just set the first byte to zero
}
bool Ip6AddrIsEmpty(const char* ip)
{
    return !ip[0]; //Check for the first byte being non zero
}
static void addHexNibble(bool* pAdded, int number, int index, char** pp)
{
    int nibble = number;
    if (index) nibble >>= 4;
    nibble &= 0xF;
    
    if (nibble || *pAdded)
    {
        **pp = nibble < 10 ? nibble + '0' : nibble - 10 + 'a';
        *pp += 1;
        *pAdded = true;
    }
}
int Ip6AddrToString(const char* pIp, int size, char* pText)
{
    const char* pIpE = pIp + 16;
    char* p = pText;
    while (true)
    {
        bool added = false;
        if (*pIp || *(pIp + 1))
        {
            if (p > pText + size - 2) { break; } addHexNibble(&added, *(pIp + 0), 1, &p);
            if (p > pText + size - 2) { break; } addHexNibble(&added, *(pIp + 0), 0, &p);
            if (p > pText + size - 2) { break; } addHexNibble(&added, *(pIp + 1), 1, &p);
            if (p > pText + size - 2) { break; } addHexNibble(&added, *(pIp + 1), 0, &p);
        }
        
        pIp += 2;
        if (pIp >= pIpE) break;
        
        if (p > pText + size - 2) break;
		*p++ = ':';
    }
    *p = 0;
    return p - pText;
}
void Ip6AddrParse(const char *pText, char *address) //Contains an empty address if invalid
{
    int field = 0;
    int word = 0;
    while(true)
    {
        switch (*pText)
        {
            case ':':
                address[field] = (word >> 8) & 0xFF;
                field++;
                if (field > 15) { address[0] = 0; return; }
                address[field] = word & 0xFF;
                field++;
                if (field > 15) { address[0] = 0; return; }
                word = 0;
                break;
            case '0': word <<= 4; word |= 0; break;
            case '1': word <<= 4; word |= 1; break;
            case '2': word <<= 4; word |= 2; break;
            case '3': word <<= 4; word |= 3; break;
            case '4': word <<= 4; word |= 4; break;
            case '5': word <<= 4; word |= 5; break;
            case '6': word <<= 4; word |= 6; break;
            case '7': word <<= 4; word |= 7; break;
            case '8': word <<= 4; word |= 8; break;
            case '9': word <<= 4; word |= 9; break;
            case 'a':
            case 'A': word <<= 4; word |= 10; break;
            case 'b':
            case 'B': word <<= 4; word |= 11; break;
            case 'c':
            case 'C': word <<= 4; word |= 12; break;
            case 'd':
            case 'D': word <<= 4; word |= 13; break;
            case 'e':
            case 'E': word <<= 4; word |= 14; break;
            case 'f':
            case 'F': word <<= 4; word |= 15; break;
            case 0:
                address[field] = (word >> 8) & 0xFF;
                field++;
                if (field != 15) { address[0] = 0; return; }
                address[field] = word & 0xFF;
                return;
            default: 
                address[0] = 0;
                return;
        }
        pText++;
    }
}
static void logHexNibble(bool* pAdded, int number, int index)
{
    int nibble = number;
    if (index) nibble >>= 4;
    nibble &= 0xF;
    
    if (nibble || *pAdded)
    {
        LogChar(nibble < 10 ? nibble + '0' : nibble - 10 + 'a');
        *pAdded = true;
    }
}
int Ip6AddrLog(const char* pIp)
{
    int count = 0;
    const char* pIpE = pIp + 16;
    while (true)
    {
        bool added = false;
        if (*pIp || *(pIp + 1))
        {
            logHexNibble(&added, *(pIp + 0), 1); if (added) count++;
            logHexNibble(&added, *(pIp + 0), 0); if (added) count++;
            logHexNibble(&added, *(pIp + 1), 1); if (added) count++;
            logHexNibble(&added, *(pIp + 1), 0); if (added) count++;
        }
        
        pIp += 2;
        if (pIp >= pIpE) break;
        
        LogChar(':'); count++;
    }
    return count;
}
static void httpHexNibble(bool* pAdded, int number, int index)
{
    int nibble = number;
    if (index) nibble >>= 4;
    nibble &= 0xF;
    
    if (nibble || *pAdded)
    {
        HttpAddChar(nibble < 10 ? nibble + '0' : nibble - 10 + 'a');
        *pAdded = true;
    }
}
int Ip6AddrHttp(const char* pIp)
{
    int count = 0;
    const char* pIpE = pIp + 16;
    while (true)
    {
        bool added = false;
        if (*pIp || *(pIp + 1))
        {
            httpHexNibble(&added, *(pIp + 0), 1); if (added) count++;
            httpHexNibble(&added, *(pIp + 0), 0); if (added) count++;
            httpHexNibble(&added, *(pIp + 1), 1); if (added) count++;
            httpHexNibble(&added, *(pIp + 1), 0); if (added) count++;
        }
        
        pIp += 2;
        if (pIp >= pIpE) break;
        
        HttpAddChar(':'); count++;
    }
    return count;
}
bool Ip6AddrIsSame(const char* ipA, const char* ipB)
{
    return !memcmp(ipA, ipB, 16); //Though about optimising by doing a reverse loop but unlikely to be faster than an optimised assembly coded library function
}
void Ip6AddrCopy(char* ipTo, const char* ipFrom)
{
    memcpy(ipTo, ipFrom, 16);
}

bool Ip6AddrIsLinkLocal(const char* p)
{
    if (p[0] != 0xFE) return false;
    if (p[1] != 0x80) return false;
    return true;
}
bool Ip6AddrIsUniqueLocal(const char* p)
{
    if (p[0] != 0xFD) return false;
    if (p[1] != 0x00) return false;
    return true;
}
bool Ip6AddrIsGlobal(const char* p)
{
    //[RFC 4291] designates 2000::/3 to be global unicast address space that the Internet Assigned Numbers Authority (IANA) may allocate to the RIRs.
    //The top byte AND 0b11100000 (0xE0)must be 0x20
    return (p[0] & 0xE0) == 0x20;
}
bool Ip6AddrIsExternal(const char* p)
{
    //Logic is address must be global and not have the global prefix
    if (!Ip6AddrIsGlobal(p)) return false;
    if (!NdpGlobalPrefixLength) return false;
    if (memcmp(NdpGlobalPrefix, p, NdpGlobalPrefixLength) != 0) return false; //Only 0 if the same
    return true;
    
}
bool Ip6AddrIsSolicited(const char* p)
{
    if (*p++ != 0xff) return false;
    if (*p++ != 0x02) return false;
    
    if (*p++ != 0x00) return false;
    if (*p++ != 0x00) return false;
    
    if (*p++ != 0x00) return false;
    if (*p++ != 0x00) return false;
    
    if (*p++ != 0x00) return false;
    if (*p++ != 0x00) return false;
    
    if (*p++ != 0x00) return false;
    if (*p++ != 0x00) return false;
    
    if (*p++ != 0x00) return false;
    if (*p++ != 0x01) return false;
    
    if (*p++ != 0xff) return false;
    
    return true;
}
bool Ip6AddrIsMulticast(const char *p)
{
    return *p == 0xFF;
}
bool Ip6AddrIsSameGroup(const char* pA, const char* pB)
{
    pA += 13;
    pB += 13;
    if (*pA++ != *pB++) return false;
    if (*pA++ != *pB++) return false;
    return *pA == *pB;
}

const char Ip6AddrAllNodes  [] = {0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
const char Ip6AddrAllRouters[] = {0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
const char Ip6AddrMdns      [] = {0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfb};
const char Ip6AddrLlmnr     [] = {0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x03};
const char Ip6AddrNtp       [] = {0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01};

void Ip6AddrFromDest(int dest, char* pDstIp)
{
    switch (dest)
    {
        case   UNICAST:        /*No change*/                                    break;
        case   UNICAST_DNS:    Ip6AddrCopy(pDstIp, NdpDnsServer           ); break;
        case   UNICAST_NTP:    Ip6AddrCopy(pDstIp, NtpClientQueryServerIp6); break;
        case   UNICAST_TFTP:   Ip6AddrCopy(pDstIp, TftpServerIp6          ); break;
        case MULTICAST_NODE:   Ip6AddrCopy(pDstIp, Ip6AddrAllNodes     ); break;
        case MULTICAST_ROUTER: Ip6AddrCopy(pDstIp, Ip6AddrAllRouters   ); break;
        case MULTICAST_MDNS:   Ip6AddrCopy(pDstIp, Ip6AddrMdns         ); break;
        case MULTICAST_LLMNR:  Ip6AddrCopy(pDstIp, Ip6AddrLlmnr        ); break;
        case MULTICAST_NTP:    Ip6AddrCopy(pDstIp, Ip6AddrNtp          ); break;
        default:
            LogTimeF("Ip6AddressFromDest unknown destination %d\r\n", dest);
            break;           
    }
}
