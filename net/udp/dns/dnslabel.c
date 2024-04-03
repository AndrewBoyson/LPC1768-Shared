#include <stdbool.h>
#include <string.h>

#include "dns.h"
#include "dnslabel.h"
#include "net/udp/dhcp/dhcp.h"

bool DnsLabelIsExternal(char* p)
{
    while (true)
    {
        if (*p == '\0') return false;
        if (*p == '.') return true;
        p++;
    }
}

void DnsLabelCopy(char* dst, char* src)
{
    strncpy(dst, src, DNS_MAX_LABEL_LENGTH); //Copies up to 63 bytes
    dst[DNS_MAX_LABEL_LENGTH] = 0; //dst[63] is the 64th position from 0..63 is set to null. All labels must be declared as a size of 64
}

bool DnsLabelIsSame(char* pA, char* pB)
{
    while(true)
    {
        char a = *pA++;
        char b = *pB++;
        if (a >= 'A' && a <= 'Z') a |= 0x20; //Make lower case
        if (b >= 'A' && b <= 'Z') b |= 0x20; //Make lower case
        if (a != b) return false;            //If different then stop and return the fact
        if (!a) break;                       //No need to check 'b' too as it will necessarily be equal to 'a' at this point.
    }
    return true;                             //If we get here the strings must equate.
}

int DnsLabelMakeFullNameFromName(int protocol, const char* p, int size, char* result)
{
    int i = 0;
    char c;
    bool isExternal = false;
    
    while (i < size - 1)
    {
        c = *p++;
        if (c == '.') isExternal = true;
        if (!c) break;
        *result++ = c;
        i++;
    }
    if (protocol == DNS_PROTOCOL_MDNS)
    {
        p = ".local";
        while (i < size - 1)
        {
            c = *p++;
            if (!c) break;
            *result++ = c;
            i++;
        }
    }
    if (protocol == DNS_PROTOCOL_UDNS && !isExternal && DhcpDomainName[0]) //Shouldn't do this in IPv6 as DHCP is IPv4 only
    {
        if (i < size - 1)
        {
            *result++ = '.';
            i++;
        }
        p = DhcpDomainName;
        while (i < size - 1)
        {
            c = *p++;
            if (!c) break;
            *result++ = c;
            i++;
        }
    }
    *result = 0; //Terminate the resulting string
    return i;
}
int DnsLabelStripNameFromFullName(int protocol, char* p, int size, char* result)
{
    int i = 0;
    char c;
    
    while (i < size - 1)
    {
        c = *p++;
        if (c == 0)   break;                               //End of the fqdn so stop
        if (c == '.')
        {
            if (protocol == DNS_PROTOCOL_UDNS)
            {
                if (strcmp(p, DhcpDomainName) == 0) break; //Strip the domain from a UDNS fqdn if, and only if, it matches the domain given in DHCP. IPv4 only.
            }
            else
            {
                break;                                     //Strip the domain from an LLMNR (there shouldn't be one) or MDNS (it should always be '.local') fqdn
            }
        }
        *result++ = c;
        i++;
    }
    *result = 0;           //Terminate the copied string
    return i;
}
