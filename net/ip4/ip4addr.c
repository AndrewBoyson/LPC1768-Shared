#include <stdint.h>
#include <stdio.h>

#include "log/log.h"
#include "web/http/http.h"
#include "net/action.h"
#include "ip4addr.h"
#include "net/udp/dhcp/dhcp.h"
#include "net/udp/ntp/ntpclient.h"
#include "net/udp/tftp/tftp.h"
#include "net/user.h"

int Ip4AddrToString(const uint32_t ip, const int size, char* text)
{
    int a0 = (ip & 0xFF000000) >> 24;
    int a1 = (ip & 0x00FF0000) >> 16;
    int a2 = (ip & 0x0000FF00) >>  8;
    int a3 = (ip & 0x000000FF);
    return snprintf(text, size, "%d.%d.%d.%d", a3, a2, a1, a0); 
}
int Ip4AddrLog(const uint32_t ip)
{
    int a0 = (ip & 0xFF000000) >> 24;
    int a1 = (ip & 0x00FF0000) >> 16;
    int a2 = (ip & 0x0000FF00) >>  8;
    int a3 = (ip & 0x000000FF);
    return LogF("%d.%d.%d.%d", a3, a2, a1, a0); 
}
int Ip4AddrHttp(const uint32_t ip)
{
    int a0 = (ip & 0xFF000000) >> 24;
    int a1 = (ip & 0x00FF0000) >> 16;
    int a2 = (ip & 0x0000FF00) >>  8;
    int a3 = (ip & 0x000000FF);
    return HttpAddF("%d.%d.%d.%d", a3, a2, a1, a0); 
}

uint32_t Ip4AddrParse(const char* pText) //Returns 0 on error
{
    const char* p = pText;
    int ints[4];
    int field = 0;
    int word = 0;
    while(true)
    {
        switch (*p)
        {
            case '.':
                ints[field] = word;
                field++;
                if (field > 3) return 0;
                word = 0;
                break;
            case '0': word *= 10; word += 0; break;
            case '1': word *= 10; word += 1; break;
            case '2': word *= 10; word += 2; break;
            case '3': word *= 10; word += 3; break;
            case '4': word *= 10; word += 4; break;
            case '5': word *= 10; word += 5; break;
            case '6': word *= 10; word += 6; break;
            case '7': word *= 10; word += 7; break;
            case '8': word *= 10; word += 8; break;
            case '9': word *= 10; word += 9; break;
            case 0:
                ints[field] = word;
                uint32_t addr4 = (ints[0] << 0) + (ints[1] << 8) + (ints[2] << 16) + (ints[3] << 24);
                return addr4;
            default: return 0;
        }
        p++;
    }
}
void Ip4AddrFromDest(const int dest, uint32_t* pDstIp)
{
    switch (dest)
    {
        case UNICAST:          /*No change*/                          break;
        case UNICAST_DNS:      *pDstIp = DhcpDnsServerIp;             break;
        case UNICAST_DHCP:     *pDstIp = DhcpServerIp;                break;
        case UNICAST_NTP:      *pDstIp = NtpClientQueryServerIp4;     break;
        case UNICAST_TFTP:     *pDstIp = TftpServerIp4;               break;
        case UNICAST_USER:     *pDstIp = UserIp4;                     break;
        case MULTICAST_NODE:   *pDstIp = IP4_MULTICAST_ALL_HOSTS;     break;
        case MULTICAST_ROUTER: *pDstIp = IP4_MULTICAST_ALL_ROUTERS;   break;
        case MULTICAST_MDNS:   *pDstIp = IP4_MULTICAST_DNS_ADDRESS;   break;
        case MULTICAST_LLMNR:  *pDstIp = IP4_MULTICAST_LLMNR_ADDRESS; break;
        case MULTICAST_NTP:    *pDstIp = IP4_MULTICAST_NTP_ADDRESS;   break;
        case BROADCAST:        *pDstIp = IP4_BROADCAST_ADDRESS;       break;
        default:
            LogTimeF("Ip4AddrFromDest unknown destination %d\r\n", dest);
            break;
    }
}
bool Ip4AddrIsExternal(uint32_t ip)
//Logic is if it isn't local and it isn't one of the three types of broadcast then it must be external.
{
    if ((ip & DhcpSubnetMask) == (DhcpLocalIp & DhcpSubnetMask)) return false; // Ip is same as local ip in the unmasked area
    if ( ip == (DhcpLocalIp | 0xFF000000)                      ) return false; // Ip == 192.168.0.255; '|' is lower precendence than '=='
    if ( ip ==  IP4_BROADCAST_ADDRESS                          ) return false; // dstIp == 255.255.255.255
    if ((ip & 0xE0) == 0xE0                                    ) return false; // 224.x.x.x == 1110 0000 == E0.xx.xx.xx == xx.xx.xx.E0 in little endian
    
    return true;
}