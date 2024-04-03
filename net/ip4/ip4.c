
#include <stdint.h>
#include <stdbool.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"
#include "icmp/icmp4.h"
#include "udptcp4.h"
#include "net/resolve/ar4.h"
#include "net/resolve/nr.h"
#include "net/udp/dhcp/dhcp.h"
#include "net/eth/eth.h"
#include "net/eth/mac.h"
#include "net/ip.h"
#include "net/ip4/ip4addr.h"
#include "net/ip4/ip4hdr.h"
#include "net/udp/ntp/ntp.h"
#include "lpc1768/reset/restart.h"
#include "net/checksum.h"

bool     Ip4Trace = true;
uint32_t Ip4Remote = 0;

#define OFF_LINK_TTL 64

static void logHeader(char* pPacket)
{
    if (NetTraceVerbose)
    {
        Log ("IP4 header\r\n");
        LogF("  Version           %d\r\n", Ip4HdrGetVersion   (pPacket));
        int headerLength =                 Ip4HdrGetHeaderLen (pPacket);
        LogF("  Header length     %d\r\n", headerLength);
        LogF("  Type of service   %d\r\n", Ip4HdrGetTos       (pPacket));
        LogF("  Total length      %d\r\n", Ip4HdrGetLength    (pPacket));
        LogF("  Identification    %d\r\n", Ip4HdrGetId        (pPacket));
        if (Ip4HdrGetDontFrag(pPacket))  LogF("  Don't fragment\r\n");
        else                             LogF("  Do fragment\r\n");
        if (Ip4HdrGetMoreFrags(pPacket)) LogF("  More fragments\r\n");
        else                             LogF("  No more fragments\r\n");
        LogF("  Frag offset       %d\r\n", Ip4HdrGetFragOffset(pPacket));
        LogF("  Time to live      %d\r\n", Ip4HdrGetTtl       (pPacket));
        LogF("  Protocol          "); IpProtocolLog(Ip4HdrGetProtocol(pPacket)); Log("\r\n");
        LogF("  Checksum (hex)    %04hX\r\n", Ip4HdrGetChecksum(pPacket));
        LogF("  Calculated (hex)  %04hX\r\n",  CheckSum(headerLength, pPacket));
        LogF("  Source IP         "); Ip4AddrLog(Ip4HdrGetSrc(pPacket)); Log("\r\n");
        LogF("  Destination IP    "); Ip4AddrLog(Ip4HdrGetDst(pPacket)); Log("\r\n");
    }
    else
    {
        Log ("IP4   header ");
        IpProtocolLog(Ip4HdrGetProtocol(pPacket));
        Log(" ");
        Ip4AddrLog(Ip4HdrGetSrc(pPacket));
        Log(" >>> ");
        Ip4AddrLog(Ip4HdrGetDst(pPacket));
        Log("\r\n");
    }
}
static void makeHeader(char* pPacket, uint16_t totalLength, uint8_t ttl, uint8_t protocol, uint32_t srcIp, uint32_t dstIp)
{
    static uint16_t id = 0;
    Ip4HdrSetVersion  (pPacket, 4                );
    Ip4HdrSetHeaderLen(pPacket, IP4_HEADER_LENGTH);
    Ip4HdrSetTos      (pPacket, 0                );
    Ip4HdrSetLength   (pPacket, totalLength      );
    Ip4HdrSetId       (pPacket, id++             ); //Used by the recipient for collating packets fragmented in transit; unique per packet sent
    Ip4HdrSetFragInfo (pPacket, 0                ); //No flags and no offset
    Ip4HdrSetTtl      (pPacket, ttl              );
    Ip4HdrSetProtocol (pPacket, protocol         );
    Ip4HdrSetSrc      (pPacket, srcIp            );
    Ip4HdrSetDst      (pPacket, dstIp            );
    Ip4HdrSetChecksum (pPacket, 0                );
    uint16_t checksum = CheckSum(IP4_HEADER_LENGTH, pPacket);
    Ip4HdrSetChecksum (pPacket, checksum         );
}
static char* traceHeader;
static void (*pTraceBack)(void);
static void trace()
{
    pTraceBack();
    logHeader(traceHeader);
}

int Ip4HandleReceivedPacket(void (*traceback)(void), char* pPacketRx, int sizeRx, char* pPacketTx, int* pSizeTx, char* macRemote)
{
    int lastRestartPoint = RestartPoint;
    RestartPoint = FAULT_POINT_Ip4HandleReceivedPacket;

    traceHeader = pPacketRx;
    pTraceBack = traceback;

    int     headerLengthRx = Ip4HdrGetHeaderLen(pPacketRx);
    uint16_t totalLengthRx = Ip4HdrGetLength   (pPacketRx);
    uint8_t  protocol      = Ip4HdrGetProtocol (pPacketRx);
    uint32_t srcIp         = Ip4HdrGetSrc      (pPacketRx);
    uint32_t dstIp         = Ip4HdrGetDst      (pPacketRx);
    
    Ip4Remote = srcIp;
    
    char* pDataRx = pPacketRx + headerLengthRx;
    char* pDataTx = pPacketTx + IP4_HEADER_LENGTH;

    if (sizeRx > totalLengthRx) sizeRx = totalLengthRx;
    int dataLengthRx =   sizeRx - headerLengthRx;
    int dataLengthTx = *pSizeTx - IP4_HEADER_LENGTH;

    bool isMe             = dstIp ==  DhcpLocalIp;
    bool isLocalBroadcast = dstIp == (DhcpLocalIp | 0xFF000000); // dstIp == 192.168.1.255; '|' is lower precendence than '=='
    bool isBroadcast      = dstIp ==  IP4_BROADCAST_ADDRESS;     // dstIp == 255.255.255.255
    bool isMulticast      = (dstIp & 0xE0) == 0xE0;              // 224.x.x.x == 1110 0000 == E0.xx.xx.xx == xx.xx.xx.E0 in little endian

    bool doIt = isMe || isLocalBroadcast || isBroadcast || isMulticast;
    if (!doIt)
    {
        if (Ip4Trace)
        {
            LogTimeF("IP4 filtered out ip "); Ip4AddrLog(dstIp);
            Log(" from ");
            Ip4AddrLog(srcIp);
            Log("\r\n");
        }
        RestartPoint = lastRestartPoint;
        return DO_NOTHING;
    }

    int remArIndex = Ar4AddIpRecord(trace, macRemote, srcIp);
    NrMakeRequestForNameFromAddress4(srcIp);

    int action = DO_NOTHING;
    switch (protocol)
    {
        case ICMP:   action = Icmp4HandleReceivedPacket(trace, pDataRx, dataLengthRx, pDataTx, &dataLengthTx, &srcIp, &dstIp);             break;
        case IGMP:                                                                                                                         break;
        case UDP:    action =  Udp4HandleReceivedPacket(trace, pDataRx, dataLengthRx, pDataTx, &dataLengthTx, &srcIp, &dstIp);             break;
        case TCP:    action =  Tcp4HandleReceivedPacket(trace, pDataRx, dataLengthRx, pDataTx, &dataLengthTx, &srcIp, &dstIp, remArIndex); break;
        case IP6IN4:                                                                                                                       break;
        default:
            LogTimeF("IP4 received packet unknown protocol %d\r\n", protocol);
            RestartPoint = lastRestartPoint;
            return DO_NOTHING;
    }
    if (!action)
    {
        RestartPoint = lastRestartPoint;
        return DO_NOTHING;
    }

    uint8_t ttl = 0;
    if (DhcpIpNeedsToBeRouted(dstIp))
    {
        Ar4IpToMac(DhcpRouterIp, macRemote);  //Send back to the router
        ttl = OFF_LINK_TTL;
    }
    else
    {
        ttl = 255;
    }

    *pSizeTx = IP4_HEADER_LENGTH + dataLengthTx;
    
    makeHeader(pPacketTx, *pSizeTx, ttl, protocol, srcIp, dstIp);
    
    if (ActionGetTracePart(action)) logHeader(pPacketTx);

    RestartPoint = lastRestartPoint;
    return action;
}
int Ip4PollForPacketToSend(char* pPacket, int* pSize, char* pDstMac)
{
    char* pData   = pPacket + IP4_HEADER_LENGTH;
    int dataLength = *pSize - IP4_HEADER_LENGTH;

    uint8_t protocol = 0;
    uint32_t   srcIp = 0;
    uint32_t   dstIp = 0;
    int      action  = DO_NOTHING;
    if (!action) { action = Udp4PollForPacketToSend(pData, &dataLength, &srcIp, &dstIp); protocol = UDP; }
    if (!action) { action = Tcp4PollForPacketToSend(pData, &dataLength, &srcIp, &dstIp); protocol = TCP; }
    if (!action) return DO_NOTHING;
    int dest = ActionGetDestPart(action);
    uint8_t ttl = 0;
    switch (dest)
    {
        case UNICAST:
        case UNICAST_DNS:
        case UNICAST_DHCP:
        case UNICAST_NTP:
        case UNICAST_TFTP:
        case UNICAST_USER:
            if (DhcpIpNeedsToBeRouted(dstIp))
            {
                Ar4IpToMac(DhcpRouterIp, pDstMac); //send via router
                ttl = OFF_LINK_TTL;
            }
            else
            {
                Ar4IpToMac(dstIp,        pDstMac); //Send direct
                ttl = 255;
            }
            break;
        case MULTICAST_NODE:
        case MULTICAST_ROUTER:
        case MULTICAST_MDNS:
        case MULTICAST_LLMNR:
        case MULTICAST_NTP:
        case BROADCAST:
            ttl = 255;
            break;
        default:
            LogTimeF("Ip4PollForPacketToSend - undefined destination %d\r\n", dest);
            return DO_NOTHING;
    }

    *pSize = IP4_HEADER_LENGTH + dataLength;
    
    makeHeader(pPacket, *pSize, ttl, protocol, srcIp, dstIp);

    if (ActionGetTracePart(action)) logHeader(pPacket);

    return action;
}
