#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "dnshdr.h"
#include "ctype.h"
#include "log/log.h"

bool DnsNameTrace = false;

#define MAX_DEPTH 10

static bool  isOffset(char c) { return (c & 0xC0) == 0xC0; }
static char* derefOffset(char* p)
{
    int offset = (*p & 0x3F) << 8; //Any bits in the upper byte are added
    p++;                           //Move to the lower byte
    offset |= *p;                  //And add it
    return DnsHdrPacket + offset;  //Return a pointer
}

int DnsNameLength(char* pStart) //Returns 0 on error
{
    char* p = pStart;
    
    while (true)
    {
        if (p >= DnsHdrData + DnsHdrDataLength)
        {
            if (DnsNameTrace) LogTimeF("DnsNameLength strayed out of the packet\r\n");
            return 0;
        }
        
        int length = *p++;
        if (length == 0)           //Its the last field so finish
        {
            return p - pStart;
        }
        else if (isOffset(length)) //it's a pointer so skip the length byte and return
        {
            p++;
            return p - pStart;
        }
        else                      //it's a length plus a field eg 3www
        {
            p += length;
        }
    }
}

bool DnsNameComparePtr(char* pStart, char* pTarget)
{
    char* p = pStart;
    
    int depth = 0;
    
    while (true)
    {
        if (p >= DnsHdrData + DnsHdrDataLength)
        {
            if (DnsNameTrace) LogTimeF("DnsNameCompare strayed out of the packet\r\n");
            return false;
        }

        int length = *p;
        if (length == 0)          //Its the last field so should be on the target's terminating NUL
        {
            if (*pTarget) return false;
            return true;
        }
        else if (isOffset(length)) //it's a pointer so go up one
        {
            depth++;
            if (depth > MAX_DEPTH)
            {
                if (DnsNameTrace) LogTimeF("DnsNameCompare exceeded depth limit\r\n");
                return false;
            }
            pStart = derefOffset(p);
            p = pStart;
        }
        else                      //it's a length plus a field eg 3www
        {
            if (p > pStart) if (*pTarget++ != '.') return false;
            p++;
            for (int i = 0; i < length; i++)
            {
                if (toupper(*pTarget++) != toupper(*p++)) return false;
            }
        }
    }
}
bool DnsNameCompareIp4(char *pStart, uint32_t ip)
{
    //char ipBytes[4];
    int field = 0;
    int depth = 0;
    char* p = pStart;
    while (true)
    {
        if (p >= DnsHdrData + DnsHdrDataLength)
        {
            LogTimeF("DnsNameCompareIp4 could not find name end\r\n");
            return false;
        }

        int length = *p;
        if (length == 0)           //Its the last field so return the ip if it is the correct length
        {
            if (field != 6) return false;
            return true;
        }
        else if (isOffset(*p))     //it's a pointer so jump to the new offset eg ^C0 ^0C
        {
            depth++;
            if (depth > MAX_DEPTH)
            {
                LogTimeF("DnsNameCompareIp4 exceeded depth limit\r\n");
                return false;
            }
            p = derefOffset(p);
        }
        else                      //it's a length plus a field eg 3www
        {
            p++;                                              // move past the length
            if (field <= 3)
            {
                if (length > 3) return false;                 // expect 90.1.168.192.in-addr.arpa
                int byte = 0;
                for (int i = 0; i < length; i++)
                {
                    byte = byte * 10;
                    if (*p > '9' || *p < '0')  return false;  // expect 90.1.168.192.in-addr.arpa
                    byte += *p - '0';
                    p++;
                }
                int ipByte = (ip >> 8 * (3 - field)) & 0xFF;
                if (byte != ipByte) return false;
            }
            else if (field == 4)
            {
                if (length != 7)  return false;               // expect 90.1.168.192.in-addr.arpa
                p+= length;
            }
            else if (field == 5)
            {
                if (length != 4)  return false;               // expect 90.1.168.192.in-addr.arpa
                p+= length;
            }
            else
            {
                 return false;                                // expect 90.1.168.192.in-addr.arpa
            }
            field++;
        }
    }
}
bool DnsNameCompareIp6(char* pStart, char* pIp)
{
    int field = 0;
    int depth = 0;
    char* p = pStart;
    while (true)
    {
        if (p >= DnsHdrData + DnsHdrDataLength)
        {
            LogTimeF("DnsNameDecodeIp6 could not find name end\r\n");
            return false;
        }

        int length = *p;
        if (length == 0)           //Its the last field so return the ip if it is the correct length
        {
            if (field != 34) return false;
            return true;
        }
        else if (isOffset(*p))     //it's a pointer so jump to the new offset eg ^C0 ^0C
        {
            depth++;
            if (depth > MAX_DEPTH)
            {
                LogTimeF("DnsNameDecodeIp6 exceeded depth limit\r\n");
                return false;
            }
            p = derefOffset(p);
        }
        else                      //it's a length plus a field eg 3www
        {
            p++;                                                     // move past the length
            if (field <= 31)
            {
                if (length > 1) return false;                        // expect 5.8.c.c.0.1.e.f.f.f.2.3.1.1.2.0.8.7.d.0.9.0.f.1.0.7.4.0.1.0.0.2.ip6.arpa
                int nibble = 0;
                if      (*p >= '0' && *p <= '9') nibble = *p - '0';
                else if (*p >= 'a' && *p <= 'f') nibble = *p - 'a' + 10;
                else if (*p >= 'A' && *p <= 'F') nibble = *p - 'A' + 10;
                else return false;                                   // expect 7.2.d.7.2.f.e.f.f.f.7.f.2.0.2.0.0.0.0.0.0.0.0.0.0.0.0.0.0.8.e.f.ip6.arpa
                int target = pIp[15 - (field >> 1)];
                if ((field & 1) == 0) { target &= 0x0F; }
                else                  { target &= 0xF0; target >>=4; }
                if (target != nibble) return false;
                p += length;
            }
            else if (field == 32)
            {
                if (length != 3) return false;                       // expect 5.8.c.c.0.1.e.f.f.f.2.3.1.1.2.0.8.7.d.0.9.0.f.1.0.7.4.0.1.0.0.2.ip6.arpa
                p+= length;
            }
            else if (field == 33)
            {
                if (length != 4) return false;                       // expect 5.8.c.c.0.1.e.f.f.f.2.3.1.1.2.0.8.7.d.0.9.0.f.1.0.7.4.0.1.0.0.2.ip6.arpa
                p+= length;
            }
            else
            {
                return false;                                        // expect 5.8.c.c.0.1.e.f.f.f.2.3.1.1.2.0.8.7.d.0.9.0.f.1.0.7.4.0.1.0.0.2.ip6.arpa
            }
            field++;
        }
    }
}

void DnsNameDecodePtr(char* pStart, char* pName, int lenName)
{   
    bool nameStarted = false;
    int depth = 0;
    char* p = pStart;
    //char* pNameStart = pName;
    while (true)
    {
        if (p >= DnsHdrData + DnsHdrDataLength)
        {
            *pName = 0;
            if (DnsNameTrace) LogTimeF("DnsNameDecodePtr could not find name end\r\n");
            return;
        }

        int length = *p;
        if (length == 0)           //Its the last field so terminate the string
        {
            *pName = 0;
            return;
        }
        else if (isOffset(*p))     //it's a pointer so jump to the new offset eg ^C0 ^0C
        {
            depth++;
            if (depth > MAX_DEPTH)
            {
                LogTimeF("DnsNameDecodePtr exceeded depth limit\r\n");
                *pName = 0;
                return;
            }
            p = derefOffset(p);
        }
        else                      //it's a length plus a field eg 3www
        {
            if (pName)
            {
                if (length + 1 > lenName) //Leave room to terminate the string.
                //Warning removed as had too many 'amzn.dmgr:9D184A89EEDE554B15EABD4931BB839B:S/FRy5cPXE:914584' alerts
                {
                    *pName = 0;
                    //LogTimeF("DnsNameDecodePtr decoded name '%s' overran name buffer trying to add %d bytes\r\n", pNameStart, length);
                    return;
                }
                lenName -= length + 1; //name chunk plus a '.'
                if (nameStarted) *pName++ = '.';
                p++;
                for (int i = 0; i < length; i++) *pName++ = *p++;
                nameStarted = true;
            }
            else
            {
                p += length + 1;
            }
        }
    }
}
void DnsNameLogPtr(char* pStart)
{   
    bool nameStarted = false;
    int depth = 0;
    char* p = pStart;
    while (true)
    {
        if (p >= DnsHdrData + DnsHdrDataLength)
        {
            if (DnsNameTrace) LogTimeF("DnsNameDecodePtr could not find name end\r\n");
            return;
        }

        int length = *p;
        if (length == 0)           //Its the last field so terminate the string
        {
            return;
        }
        else if (isOffset(*p))     //it's a pointer so jump to the new offset eg ^C0 ^0C
        {
            depth++;
            if (depth > MAX_DEPTH)
            {
                LogTimeF("DnsNameDecodePtr exceeded depth limit\r\n");
                return;
            }
            p = derefOffset(p);
        }
        else                      //it's a length plus a field eg 3www
        {
            if (nameStarted) LogChar('.');
            p++;
            for (int i = 0; i < length; i++) LogChar(*p++);
            nameStarted = true;
        }
    }
}
void DnsNameDecodeIp4(char* pStart, uint32_t* pIp)
{
    int field = 0;
    int depth = 0;
    char* p = pStart;
    while (true)
    {
        if (p >= DnsHdrData + DnsHdrDataLength)
        {
            *pIp = 0;
            LogTimeF("DnsNameDecodeIp4 could not find name end\r\n");
            return;
        }

        int length = *p;
        if (length == 0)           //Its the last field so return the ip if it is the correct length
        {
            if (field != 6) *pIp = 0;
            return;
        }
        else if (isOffset(*p))     //it's a pointer so jump to the new offset eg ^C0 ^0C
        {
            depth++;
            if (depth > MAX_DEPTH)
            {
                LogTimeF("DnsNameDecodeIp4 exceeded depth limit\r\n");
                *pIp = 0;
                return;
            }
            p = derefOffset(p);
        }
        else                      //it's a length plus a field eg 3www
        {
            p++;                                                   // move past the length
            if (field <= 3)
            {
                if (length > 3) {*pIp = 0; return; }               // expect 90.1.168.192.in-addr.arpa
                int byte = 0;
                for (int i = 0; i < length; i++)
                {
                    byte = byte * 10;
                    if (*p > '9' || *p < '0')  {*pIp = 0; return; } // expect 90.1.168.192.in-addr.arpa
                    byte += *p - '0';
                    p++;
                }
                *pIp <<= 8;
                *pIp |= byte;
            }
            else if (field == 4)
            {
                if (length != 7)  {*pIp = 0; return; }              // expect 90.1.168.192.in-addr.arpa
                p+= length;
            }
            else if (field == 5)
            {
                if (length != 4)  {*pIp = 0; return; }              // expect 90.1.168.192.in-addr.arpa
                p+= length;
            }
            else
            {
                 *pIp = 0; return;                                  // expect 90.1.168.192.in-addr.arpa
            }
            field++;
        }
    }
}
void DnsNameDecodeIp6(char* pStart, char* pIp)
{
    int field = 0;
    int depth = 0;
    char* p = pStart;
    while (true)
    {
        if (p >= DnsHdrData + DnsHdrDataLength)
        {
            pIp[0] = 0;
            LogTimeF("DnsNameDecodeIp6 could not find name end\r\n");
            return;
        }

        int length = *p;
        if (length == 0)           //Its the last field so return the ip if it is the correct length
        {
            if (field != 34) pIp[0] = 0;
            return;
        }
        else if (isOffset(*p))     //it's a pointer so jump to the new offset eg ^C0 ^0C
        {
            depth++;
            if (depth > MAX_DEPTH)
            {
                LogTimeF("DnsNameDecodeIp6 exceeded depth limit\r\n");
                *pIp = 0;
                return;
            }
            p = derefOffset(p);
        }
        else                      //it's a length plus a field eg 3www
        {
            p++;                                                     // move past the length
            if (field <= 31)
            {
                if (length > 1) {pIp[0] = 0; return; }               // expect 5.8.c.c.0.1.e.f.f.f.2.3.1.1.2.0.8.7.d.0.9.0.f.1.0.7.4.0.1.0.0.2.ip6.arpa
                int nibble = 0;
                if      (*p >= '0' && *p <= '9') nibble = *p - '0';
                else if (*p >= 'a' && *p <= 'f') nibble = *p - 'a' + 10;
                else if (*p >= 'A' && *p <= 'F') nibble = *p - 'A' + 10;
                else {pIp[0] = 0; return; }                           // expect 5.8.c.c.0.1.e.f.f.f.2.3.1.1.2.0.8.7.d.0.9.0.f.1.0.7.4.0.1.0.0.2.ip6.arpa
                if ((field & 1) == 0)
                {
                    pIp[15 - (field >> 1)]  = nibble;
                }
                else
                {
                    nibble <<= 4;
                    pIp[15 - (field >> 1)] |= nibble;
                }
                p += length;
            }
            else if (field == 32)
            {
                if (length != 3) {pIp[0] = 0; return; }              // expect 5.8.c.c.0.1.e.f.f.f.2.3.1.1.2.0.8.7.d.0.9.0.f.1.0.7.4.0.1.0.0.2.ip6.arpa
                p+= length;
            }
            else if (field == 33)
            {
                if (length != 4) {pIp[0] = 0; return; }              // expect 5.8.c.c.0.1.e.f.f.f.2.3.1.1.2.0.8.7.d.0.9.0.f.1.0.7.4.0.1.0.0.2.ip6.arpa
                p+= length;
            }
            else
            {
                pIp[0] = 0; return;                                   // expect 5.8.c.c.0.1.e.f.f.f.2.3.1.1.2.0.8.7.d.0.9.0.f.1.0.7.4.0.1.0.0.2.ip6.arpa
            }
            field++;
        }
    }
}
void DnsNameEncodeIp4(uint32_t ip, char** pp)
{
    char* p = *pp; //Get a convenient pointer to the next byte
    
    char* pLen = p;
    p++;
    
    *pLen = sprintf(p, "%lu", (ip & 0xff000000) >> 24);
    p += *pLen;
    pLen = p;
    p++;
    
    *pLen = sprintf(p, "%lu", (ip & 0x00ff0000) >> 16);
    p += *pLen;
    pLen = p;
    p++;
    
    *pLen = sprintf(p, "%lu", (ip & 0x0000ff00) >>  8);
    p += *pLen;
    pLen = p;
    p++;

    *pLen = sprintf(p, "%lu", (ip & 0x000000ff) >>  0);
    p += *pLen;
    pLen = p;
    p++;
    
    *pLen = sprintf(p, "in-addr");
    p += *pLen;
    pLen = p;
    p++;
    
    *pLen = sprintf(p, "arpa");
    p += *pLen;
    pLen = p;
    p++;
    
    *pLen = 0;

    *pp = p; //Save the convenient pointer back into the pointer to pointer
    
}

void DnsNameEncodeIp6(char* ip, char** pp)
{
    char* p = *pp; //Get a convenient pointer to the next byte
        
    ip += 16;
    for (int i = 0; i < 16; i++)
    {
        ip--;
        
        int v;
                
        *p++ = 1;
        v = *ip & 0x0F;
        *p++ = v < 10 ? v + '0' : v - 10 + 'a';

        *p++ = 1;
        v = *ip >> 4;
        *p++ = v < 10 ? v + '0' : v - 10 + 'a';
    }
    char* pLen = p;
    p++;

    *pLen = sprintf(p, "ip6");
    p += *pLen;
    pLen = p;
    p++;
    
    *pLen = sprintf(p, "arpa");
    p += *pLen;
    pLen = p;
    p++;
    
    *pLen = 0;

    *pp = p; //Save the convenient pointer back into the pointer to pointer
    
}

void DnsNameEncodePtr(char* pName, char** pp)
{
    char* p = *pp; //Get a convenient pointer to the next byte
    
    char* pLen = p; //Record the position of the first length byte
    p++;            //Move to the start of the first field
    
    while (true)
    {
        char c = *pName;
        switch (c)
        {
            case 0:
                *pLen = p - pLen - 1;
                *p = 0;
                p++;
                *pp = p; //Save the convenient pointer back into the pointer to pointer
                return;
            case '.':
                *pLen = p - pLen - 1;
                pLen = p;
                p++;
                break;
            default:
                *p = c;
                p++;
                break;
        }
        pName++;
    }
}
