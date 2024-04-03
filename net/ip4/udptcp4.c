#include <stdint.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"
#include "net/udp/dhcp/dhcp.h"
#include "net/eth/eth.h"
#include "net/ip.h"
#include "net/ip4/ip4addr.h"
#include "net/tcp/tcp.h"
#include "net/tcp/tcprecv.h"
#include "net/tcp/tcpsend.h"
#include "net/tcp/tcphdr.h"
#include "net/udp/udp.h"
#include "net/resolve/ar4.h"
#include "lpc1768/led.h"
#include "lpc1768/reset/restart.h"
#include "net/checksum.h"

static uint16_t calculateChecksum(uint8_t pro, uint32_t srcIp, uint32_t dstIp, int size, void* pPacket)
{
    uint32_t sum = 0;
    uint16_t pro16 = pro;
     sum = CheckSumAddDirect(sum,    4, &srcIp );
     sum = CheckSumAddDirect(sum,    4, &dstIp );
     sum = CheckSumAddInvert(sum,    2, &pro16 );
     sum = CheckSumAddInvert(sum,    2, &size  );
    return CheckSumFinDirect(sum, size, pPacket);
}
static void finalisePacket(uint8_t pro, int action, void* pPacket, int size, uint32_t* pSrcIp, uint32_t* pDstIp)
{    
    if (!action) return;
    
    Ip4AddrFromDest(ActionGetDestPart(action), pDstIp);
    *pSrcIp = DhcpLocalIp;
    
    switch (pro)
    {
        case TCP: TcpHdrWriteToPacket(pPacket); break;
        case UDP: UdpMakeHeader(size, pPacket); break;
    }
    
    uint16_t checksum = calculateChecksum(pro, *pSrcIp, *pDstIp, size, pPacket);
    
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
        default: LogTimeF("UdpTcp4 - traceback unrecognised protocol %d\r\n", tracePacketProtocol); break;
    }
}
int Tcp4HandleReceivedPacket(void (*traceback)(void), void* pPacketRx, int sizeRx, void* pPacketTx, int* pSizeTx, uint32_t* pSrcIp, uint32_t* pDstIp, int remArIndex)
{
    int lastRestartPoint = RestartPoint;
    RestartPoint = FAULT_POINT_Tcp4HandleReceivedPacket;
    
    pTraceBack = traceback;
    tracePacketProtocol = TCP;
    calculatedChecksum = calculateChecksum(TCP, *pSrcIp, *pDstIp, sizeRx, pPacketRx);
        
    int action = TcpHandleReceivedPacket(trace, sizeRx, pPacketRx, pSizeTx, pPacketTx, ETH_IPV4, remArIndex, 0);
    
    *pDstIp = *pSrcIp;
    
    finalisePacket(TCP, action, pPacketTx, *pSizeTx, pSrcIp, pDstIp);
  
    RestartPoint = lastRestartPoint;
    return action;
}

int Udp4HandleReceivedPacket(void (*traceback)(void), void* pPacketRx, int sizeRx, void* pPacketTx, int* pSizeTx, uint32_t* pSrcIp, uint32_t* pDstIp)
{   
    int lastRestartPoint = RestartPoint;
    RestartPoint = FAULT_POINT_Udp4HandleReceivedPacket;
    
    pTraceBack = traceback;
    tracePacketProtocol = UDP;
    calculatedChecksum = calculateChecksum(UDP, *pSrcIp, *pDstIp, sizeRx, pPacketRx);
     
    int action = UdpHandleReceivedPacket(trace, sizeRx, pPacketRx, pSizeTx, pPacketTx);

    *pDstIp = *pSrcIp;
    
    finalisePacket(UDP, action, pPacketTx, *pSizeTx, pSrcIp, pDstIp); //Note that the ports are reversed here
    
    RestartPoint = lastRestartPoint;
    return action;
}
int Tcp4PollForPacketToSend(void* pPacket, int* pSize, uint32_t* pSrcIp, uint32_t* pDstIp)
{        
    int remArIndex = -1;
    int action = TcpPollForPacketToSend(pSize, pPacket, ETH_IPV4, &remArIndex, NULL);
    if (action && remArIndex >= 0) *pDstIp = Ar4IndexToIp(remArIndex);
    
    finalisePacket(TCP, action, pPacket, *pSize, pSrcIp, pDstIp);
        
    return action;
}
int Udp4PollForPacketToSend(void* pPacket, int* pSize, uint32_t* pSrcIp, uint32_t* pDstIp)
{        
    int action = UdpPollForPacketToSend(ETH_IPV4, pSize, pPacket);
    
    finalisePacket(UDP, action, pPacket, *pSize, pSrcIp, pDstIp);
        
    return action;
}
