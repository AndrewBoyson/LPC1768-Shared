#include <stdint.h>
#include <stdbool.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"
#include "net/ip6/icmp/icmp6.h"
#include "net/ip6/udptcp6.h"
#include "net/resolve/ar6.h"
#include "net/resolve/nr.h"
#include "net/ip6/slaac.h"
#include "net/eth/eth.h"
#include "net/ip.h"
#include "net/ip6/ip6addr.h"
#include "net/ip6/ip6hdr.h"
#include "net/ip6/icmp/ndp/ndp.h"
#include "net/udp/ntp/ntp.h"
#include "net/eth/mac.h"
#include "web/http/http.h"

bool Ip6Trace = true;

static void logHeader(char* pPacket)
{
    if (NetTraceVerbose)
    {
        Log("IP6 header\r\n");
        LogF("  Version           %d\r\n",          Ip6HdrGetVersion   (pPacket));
        LogF("  Payload length    %d\r\n",          Ip6HdrGetPayloadLen(pPacket));
        LogF("  Hop limit         %d\r\n",          Ip6HdrGetHopLimit  (pPacket));
        LogF("  Protocol          "); IpProtocolLog(Ip6HdrGetProtocol  (pPacket)); Log("\r\n");
        Log ("  Source IP         "); Ip6AddrLog   (Ip6HdrPtrSrc       (pPacket)); Log("\r\n");
        Log ("  Destination IP    "); Ip6AddrLog   (Ip6HdrPtrDst       (pPacket)); Log("\r\n");
    }
    else
    {
        Log("IP6   header ");
        IpProtocolLog(Ip6HdrGetProtocol(pPacket));
        Log(" ");
        Ip6AddrLog   (Ip6HdrPtrSrc     (pPacket));
        Log(" >>> ");
        Ip6AddrLog   (Ip6HdrPtrDst     (pPacket));
        Log("\r\n");
    }
}
static char* pTracePacket;
static void (*pTraceBack)(void);
static void trace()
{
    pTraceBack();
    logHeader(pTracePacket);
}
int Ip6HandleReceivedPacket(void (*traceback)(void), char* pPacketRx, int sizeRx, char* pPacketTx, int* pSizeTx, char* macRemote)
{    
    pTracePacket = pPacketRx;
    pTraceBack   = traceback;
    
    char* pDataRx = pPacketRx + IP6_HEADER_LENGTH;
    char* pDataTx = pPacketTx + IP6_HEADER_LENGTH;
    
    int protocol        = Ip6HdrGetProtocol  (pPacketRx);
    int payloadLengthRx = Ip6HdrGetPayloadLen(pPacketRx);
    
    static char srcIp[16];
    static char dstIp[16];
    Ip6AddrCopy(srcIp, Ip6HdrPtrSrc (pPacketRx));
    Ip6AddrCopy(dstIp, Ip6HdrPtrDst (pPacketRx));
    
    int dataLengthRx = sizeRx - IP6_HEADER_LENGTH;
    if (dataLengthRx > payloadLengthRx) dataLengthRx = payloadLengthRx; //Choose the lesser of the data length and the payload length
    int dataLengthTx = *pSizeTx - IP6_HEADER_LENGTH;
    
    int  scope       = SlaacScope(dstIp);
    bool isMe        = scope != SCOPE_NONE;
    bool isMulticast = Ip6AddrIsMulticast(dstIp);
    bool isSolicited = Ip6AddrIsSolicited(dstIp);
    bool isGroup     = Ip6AddrIsSameGroup(dstIp, SlaacLinkLocalIp);
    
    bool doIt = isMe || (isMulticast && !isSolicited) || (isGroup && isSolicited);
    
    if (!doIt)
    {
        if (Ip6Trace)
        {
            LogTime("IP6 filtered out ip ");
            Ip6AddrLog(dstIp);
            LogF(" from ");
            Ip6AddrLog(srcIp);
            Log("\r\n");
        }
        return DO_NOTHING;
    }
    
    NetTraceHostCheckIp6(srcIp);
    
    int remArIndex = Ar6AddIpRecord(trace, macRemote, srcIp);
    NrMakeRequestForNameFromAddress6(srcIp);

    int action = DO_NOTHING;
    switch (protocol)
    {
        case HOPOPT: action = DO_NOTHING;                                                                                           break;
        case ICMP6:  action = Icmp6HandleReceivedPacket(trace, scope, pDataRx, dataLengthRx, pDataTx, &dataLengthTx, srcIp, dstIp); break;
        case UDP:    action =  Udp6HandleReceivedPacket(trace, scope, pDataRx, dataLengthRx, pDataTx, &dataLengthTx, srcIp, dstIp); break;
        case TCP:    action =  Tcp6HandleReceivedPacket(trace, scope, pDataRx, dataLengthRx, pDataTx, &dataLengthTx, srcIp, dstIp, remArIndex); break;        
        default:
            LogTimeF("IP6 protocol %d unhandled\r\n", protocol);
            return DO_NOTHING;
    }
    if (!action) return DO_NOTHING;
    
    int hoplimit;
    if (Ip6AddrIsExternal(dstIp))
    {
        MacCopy(macRemote, NdpRouterMac); //Send to the router MAC
        hoplimit = NdpHopLimit;
    }
    else
    {
        hoplimit = 255;
    }
    
    Ip6HdrSetVersion   (pPacketTx, 6           );
    Ip6HdrSetPayloadLen(pPacketTx, dataLengthTx);
    Ip6HdrSetProtocol  (pPacketTx, protocol    );
    Ip6HdrSetHopLimit  (pPacketTx, hoplimit    );
    
    Ip6AddrCopy(Ip6HdrPtrSrc(pPacketTx), srcIp);
    Ip6AddrCopy(Ip6HdrPtrDst(pPacketTx), dstIp);
      
    *pSizeTx = IP6_HEADER_LENGTH + dataLengthTx;
    
    if (ActionGetTracePart(action)) logHeader(pPacketTx);

    return action;
}
int Ip6PollForPacketToSend(char* pPacket, int* pSize, char* pDstMac)
{    
    static char srcIp[16];
    static char dstIp[16];
    
    char* pData    = pPacket + IP6_HEADER_LENGTH;
    int dataLength = *pSize  - IP6_HEADER_LENGTH;
    
    int protocol = 0;
    int action = DO_NOTHING;
    if (!action) { action = Icmp6PollForPacketToSend(pData, &dataLength, srcIp, dstIp); protocol = ICMP6; }
    if (!action) { action =  Udp6PollForPacketToSend(pData, &dataLength, srcIp, dstIp); protocol = UDP;   }
    if (!action) { action =  Tcp6PollForPacketToSend(pData, &dataLength, srcIp, dstIp); protocol = TCP;   }
    if (!action) return DO_NOTHING;
    
    int hoplimit = 0;
    int dest = ActionGetDestPart(action);
    switch (dest)
    {
        case UNICAST:
        case UNICAST_DNS:
        case UNICAST_DHCP:
        case UNICAST_NTP:
        case UNICAST_TFTP:
            if (Ip6AddrIsExternal(dstIp))
            {
                MacCopy(pDstMac, NdpRouterMac); //Send to the router MAC
                hoplimit = NdpHopLimit;
            }
            else
            {
                Ar6IpToMac(dstIp, pDstMac); //Make the remote MAC from NP
                hoplimit = 255;
            }
            break;
        case MULTICAST_NODE:
        case MULTICAST_ROUTER:
        case MULTICAST_MDNS:
        case MULTICAST_LLMNR:
        case MULTICAST_NTP:
        case SOLICITED_NODE:
            hoplimit = 255;
            break;
        default:
            LogTimeF("Ip6PollForPacketToSend - undefined destination %d\r\n", dest);
            return DO_NOTHING;
    }

    Ip6HdrSetVersion   (pPacket, 6         );
    Ip6HdrSetPayloadLen(pPacket, dataLength);
    Ip6HdrSetProtocol  (pPacket, protocol  );
    Ip6HdrSetHopLimit  (pPacket, hoplimit  );
    Ip6AddrCopy(Ip6HdrPtrSrc(pPacket), srcIp);
    Ip6AddrCopy(Ip6HdrPtrDst(pPacket), dstIp);

    *pSize = IP6_HEADER_LENGTH + dataLength;
    
    if (ActionGetTracePart(action)) logHeader(pPacket);

    return action;
}
