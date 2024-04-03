#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"
#include "eth.h"
#include "web/http/http.h"

char MacLocal[6];
void MacClear(char* mac)
{
    memset(mac, 0, 6);
}
void MacCopy(char* macTo, const char* macFrom)
{
    memcpy(macTo, macFrom, 6);
}
bool MacIsSame(const char* macA, const char* macB)
{
    return memcmp(macA, macB, 6) == 0;
}
bool MacIsEmpty(const char* mac)
{
           if (*mac) return false;
    mac++; if (*mac) return false;
    mac++; if (*mac) return false;
    mac++; if (*mac) return false;
    mac++; if (*mac) return false;
    mac++; if (*mac) return false;
    return true;
}

void MacParse(const char *text, char *mac)
{
    MacClear(mac);
    int field = 0;
    int byte = 0;
    while(true)
    {
        switch (*text)
        {
            case ':':
                mac[field] = byte;
                field++;
                if (field > 6) return;
                byte = 0;
                break;
            case '0': byte <<= 4; byte |= 0; break;
            case '1': byte <<= 4; byte |= 1; break;
            case '2': byte <<= 4; byte |= 2; break;
            case '3': byte <<= 4; byte |= 3; break;
            case '4': byte <<= 4; byte |= 4; break;
            case '5': byte <<= 4; byte |= 5; break;
            case '6': byte <<= 4; byte |= 6; break;
            case '7': byte <<= 4; byte |= 7; break;
            case '8': byte <<= 4; byte |= 8; break;
            case '9': byte <<= 4; byte |= 9; break;
            case 'a':
            case 'A': byte <<= 4; byte |= 10; break;
            case 'b':
            case 'B': byte <<= 4; byte |= 11; break;
            case 'c':
            case 'C': byte <<= 4; byte |= 12; break;
            case 'd':
            case 'D': byte <<= 4; byte |= 13; break;
            case 'e':
            case 'E': byte <<= 4; byte |= 14; break;
            case 'f':
            case 'F': byte <<= 4; byte |= 15; break;
            case 0:
                mac[field] = byte;
                return;
        }
        text++;
    }
}
int MacToString(const char* mac, int size, char* text)
{
    return snprintf(text, size, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
int MacLog(const char* mac)
{
    return LogF("%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
int MacHttp(const char* mac)
{
    return HttpAddF("%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

int MacAccept(const char* p)
{
    switch (*p)
    {
        case 0xff: //Broadcast
        {
            p++; if (*p != 0xff) return DO_NOTHING;
            p++; if (*p != 0xff) return DO_NOTHING;
            p++; if (*p != 0xff) return DO_NOTHING;
            p++; if (*p != 0xff) return DO_NOTHING;
            p++; if (*p != 0xff) return DO_NOTHING;
            return BROADCAST;
        }
        case 0x01: //Multicast IP4
        {
            p++; if (*p != 0x00) return DO_NOTHING; //01 00
            p++; if (*p != 0x5e) return DO_NOTHING; //01 00 5E
            p++; if (*p != 0x00) return DO_NOTHING; //01 00 5E 00
            p++;
            switch (*p)
            {
                case 0x00: //01 00 5E 00 00
                    p++;
                    switch (*p) //01 00 5E 00 00 xx
                    {
                        case 0x01: return MULTICAST_NODE;
                        case 0x02: return MULTICAST_ROUTER;
                        case 0xfb: return MULTICAST_MDNS;
                        case 0xfc: return MULTICAST_LLMNR;
                        default:   return DO_NOTHING;
                    }
                case 0x01: //01 00 5E 00 01
                    p++;
                    switch (*p) //01 00 5E 00 01 xx
                    {
                        case 0x01: return MULTICAST_NTP;
                        default:   return DO_NOTHING;
                    }
                default: return DO_NOTHING;
            }
        }
        case 0x33: //Multicast IP6
        {
            p++; if (*p != 0x33) return DO_NOTHING;
            p++;
            switch (*p) //33 33
            {
                case 0x00: //33 33 00 Special address
                {
                    p++;
                    switch (*p)
                    {
                        case 0x00: //33 33 00 00
                        {
                            p++; 
                            switch (*p)
                            {
                                case 0x00:
                                    p++;
                                    switch (*p) //33 33 00 00 00 xx
                                    {
                                        case 0x01: return MULTICAST_NODE;
                                        case 0x02: return MULTICAST_ROUTER;
                                        case 0xfb: return MULTICAST_MDNS;
                                        default:   return DO_NOTHING;
                                    }
                                case 0x01:
                                    p++;
                                    switch (*p) //33 33 00 00 01 xx
                                    {
                                        case 0x01: return MULTICAST_NTP;
                                        default:   return DO_NOTHING;
                                    }
                                default: return DO_NOTHING;
                            }
                        }
                        case 0x01: //33 33 00 01
                        {
                            p++; if (*p != 0x00) return DO_NOTHING;
                            p++;
                            switch (*p) //33 33 00 01 00 xx
                            {
                                case 0x03: return MULTICAST_LLMNR;
                                default:   return DO_NOTHING;
                            }
                        }
                        default: return DO_NOTHING;
                    }
                }
                case 0xff: //33 33 FF LL LL LL Solicited address
                {
                    char* q = MacLocal + 3;
                    p++; if (*p != *q++) return DO_NOTHING;
                    p++; if (*p != *q++) return DO_NOTHING;
                    p++; if (*p != *q++) return DO_NOTHING;
                    return SOLICITED_NODE;
                }
                default: return DO_NOTHING;
            }
        }
        default: //Unicast to me
        {
            char* q = MacLocal;
                 if (*p != *q++) return DO_NOTHING;
            p++; if (*p != *q++) return DO_NOTHING;
            p++; if (*p != *q++) return DO_NOTHING;
            p++; if (*p != *q++) return DO_NOTHING;
            p++; if (*p != *q++) return DO_NOTHING;
            p++; if (*p != *q)   return DO_NOTHING;
            return UNICAST;
        }
    }
}

void MacMakeFromDest(int dest, int pro, char* p)
{
    if (dest == BROADCAST)       { *p++ = 0xff; *p++ = 0xff; *p++ = 0xff; *p++ = 0xff; *p++ = 0xff; *p = 0xff; return; }
    
    if (pro == ETH_IPV4)
    {
        switch (dest)
        {
            case MULTICAST_NODE:   *p++ = 0x01; *p++ = 0x00; *p++ = 0x5e; *p++ = 0x00; *p++ = 0x00; *p = 0x01; break;
            case MULTICAST_ROUTER: *p++ = 0x01; *p++ = 0x00; *p++ = 0x5e; *p++ = 0x00; *p++ = 0x00; *p = 0x02; break;
            case MULTICAST_MDNS:   *p++ = 0x01; *p++ = 0x00; *p++ = 0x5e; *p++ = 0x00; *p++ = 0x00; *p = 0xfb; break;
            case MULTICAST_LLMNR:  *p++ = 0x01; *p++ = 0x00; *p++ = 0x5e; *p++ = 0x00; *p++ = 0x00; *p = 0xfc; break;
            case MULTICAST_NTP:    *p++ = 0x01; *p++ = 0x00; *p++ = 0x5e; *p++ = 0x00; *p++ = 0x01; *p = 0x01; break;
        }
    }
    else
    {
        switch (dest)
        {
            case MULTICAST_NODE:   *p++ = 0x33; *p++ = 0x33; *p++ = 0x00; *p++ = 0x00; *p++ = 0x00; *p = 0x01; break;
            case MULTICAST_ROUTER: *p++ = 0x33; *p++ = 0x33; *p++ = 0x00; *p++ = 0x00; *p++ = 0x00; *p = 0x02; break;
            case MULTICAST_MDNS:   *p++ = 0x33; *p++ = 0x33; *p++ = 0x00; *p++ = 0x00; *p++ = 0x00; *p = 0xfb; break;
            case MULTICAST_LLMNR:  *p++ = 0x33; *p++ = 0x33; *p++ = 0x00; *p++ = 0x01; *p++ = 0x00; *p = 0x03; break;
            case MULTICAST_NTP:    *p++ = 0x33; *p++ = 0x33; *p++ = 0x00; *p++ = 0x00; *p++ = 0x01; *p = 0x01; break;
        }
    }
}
