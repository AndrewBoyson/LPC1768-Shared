#include <stdint.h>
#include <string.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"
#include "net/eth/eth.h"
#include "net/udp/dhcp/dhcp.h"
#include "net/ip6/icmp/ndp/ns.h"
#include "net/ip.h"
#include "net/ip6/ip6addr.h"
#include "net/ip6/slaac.h"
#include "net/tcp/tcp.h"
#include "net/tcp/tcprecv.h"
#include "net/tcp/tcpsend.h"
#include "net/tcp/tcphdr.h"
#include "net/udp/udp.h"
#include "net/resolve/ar6.h"
#include "net/checksum.h"
#include "lpc1768/led.h"

static uint16_t calculateChecksum(uint8_t pro, char* pSrcIp, char* pDstIp, int size, void* pPacket)
{   
    uint32_t sum = 0;
    uint16_t pro16 = pro;
     sum = CheckSumAddDirect(sum,   16, pSrcIp );
     sum = CheckSumAddDirect(sum,   16, pDstIp );
     sum = CheckSumAddInvert(sum,    2, &pro16 );
     sum = CheckSumAddInvert(sum,    2, &size  );
    return CheckSumFinDirect(sum, size, pPacket);
}
static void finalisePacket(uint8_t pro, int action, int scope, void* pPacket, int size, char* pSrcIp, char* pDstIp)
{    
    if (!action) return;
    
      Ip6AddrFromDest    (ActionGetDestPart(action), pDstIp);
    SlaacAddressFromScope(scope,                     pSrcIp);
        
    switch (pro)
    {
        case TCP: TcpHdrWriteToPacket(pPacket); break;
        case UDP: UdpMakeHeader(size, pPacket); break;
    }
    
    uint16_t checksum = calculateChecksum(pro, pSrcIp, pDstIp, size, pPacket);
    
    switch (pro)
    {
        case TCP: TcpHdrSetChecksum(pPacket, checksum); break;
        case UDP: UdpHdrSetChecksum(pPacket, checksum); break;
    }
    
    if (ActionGetTracePart(action))
    {
        switch (pro)
        {
            case TCP: TcpHdrLog(0); break;
            case UDP: UdpLogHeader(0); break;
        }
    }
}
static void (*pTraceBack)(void);
static int tracePacketProtocol;
static uint16_t calculatedChecksum;
static void trace()
{
    pTraceBack();
    switch(tracePacketProtocol)
    {
        case UDP: UdpLogHeader(calculatedChecksum); break;
        case TCP: TcpHdrLog(calculatedChecksum); break;
        default: LogTimeF("UdpTcp6 - traceback unrecognised protocol %d\r\n", tracePacketProtocol); break;
    }
}

int Tcp6HandleReceivedPacket(void (*traceback)(void), int scope, void* pPacketRx, int sizeRx, void* pPacketTx, int* pSizeTx, char* pSrcIp, char* pDstIp, int remArIndex)
{
    pTraceBack = traceback;
    tracePacketProtocol = TCP;
    calculatedChecksum = calculateChecksum(TCP, pSrcIp, pDstIp, sizeRx, pPacketRx);
    
    int action = TcpHandleReceivedPacket(trace, sizeRx, pPacketRx, pSizeTx, pPacketTx, ETH_IPV6, remArIndex, scope);
    
    Ip6AddrCopy(pDstIp, pSrcIp);
    
    finalisePacket(TCP, action, scope, pPacketTx, *pSizeTx, pSrcIp, pDstIp);
        
    return action;
}

int Udp6HandleReceivedPacket(void (*traceback)(void), int scope, void* pPacketRx, int sizeRx, void* pPacketTx, int* pSizeTx, char* pSrcIp, char* pDstIp)
{
    pTraceBack = traceback;
    tracePacketProtocol = UDP;
    calculatedChecksum = calculateChecksum(UDP, pSrcIp, pDstIp, sizeRx, pPacketRx);
        
    int action = UdpHandleReceivedPacket(trace, sizeRx, pPacketRx, pSizeTx, pPacketTx);
        
    Ip6AddrCopy(pDstIp, pSrcIp);
    
    finalisePacket(UDP, action, scope, pPacketTx, *pSizeTx, pSrcIp, pDstIp);
        
    return action;
}
int Tcp6PollForPacketToSend(void* pPacket, int* pSize, char* pSrcIp, char* pDstIp)
{
    int remArIndex = -1;
    int locIpScope = SCOPE_LINK_LOCAL;
    int action = TcpPollForPacketToSend(pSize, pPacket, ETH_IPV6, &remArIndex, &locIpScope);
    if (action && remArIndex >= 0) Ar6IndexToIp(remArIndex, pDstIp);
    

    finalisePacket(TCP, action, locIpScope, pPacket, *pSize, pSrcIp, pDstIp);
                
    return action;

}
int Udp6PollForPacketToSend(void* pPacket, int* pSize, char* pSrcIp, char* pDstIp)
{        
    int action = UdpPollForPacketToSend(ETH_IPV6, pSize, pPacket);
    
    int scope = SCOPE_LINK_LOCAL;

    finalisePacket(UDP, action, scope, pPacket, *pSize, pSrcIp, pDstIp);
                
    return action;

}

