#include <stdint.h>
#include "log/log.h"
#include "net/net.h"
#include "net/action.h"
#include "net/arp/arp.h"
#include "net/ip4/ip4.h"
#include "net/ip6/ip6.h"
#include "net/link/link.h"
#include "eth.h"
#include "mac.h"
#include "lpc1768/reset/restart.h"
#include "lpc1768/led.h"

#define MTU 1500

uint16_t EthProtocol; //Set when receiving or sending packets so that higher levels can read the protocol in use
char*    EthMacRemote; //Set when receiving packets so that higher levels can read the protocol in use

//header variables
static char*    hdrDstPtr(char* pPacket) { return pPacket +  0; }
static char*    hdrSrcPtr(char* pPacket) { return pPacket +  6; }
static char*    hdrTypPtr(char* pPacket) { return pPacket + 12; }
#define HEADER_LENGTH 14
static uint16_t hdrTypGet(char* pPacket) { uint16_t res;     NetInvert16(&res, hdrTypPtr(pPacket)        ); return res; }
static void     hdrTypSet(char* pPacket,   uint16_t value) { NetInvert16(      hdrTypPtr(pPacket), &value); }

void EthProtocolLog(uint16_t protocol)
{
    switch (protocol)
    {
        case ETH_ARP:  Log("ARP");              break;
        case ETH_IPV4: Log("IPV4");             break;
        case ETH_IPV6: Log("IPV6");             break;
        default:       LogF("%04hX", protocol); break;
    }
}
void LogHeader(char* pPacket)
{
    if (NetTraceVerbose)
    {
        Log("ETH header\r\n");
        Log("  Destination:  ");         MacLog(hdrDstPtr(pPacket)); Log("\r\n");
        Log("  Source:       ");         MacLog(hdrSrcPtr(pPacket)); Log("\r\n");
        Log("  EtherType:    "); EthProtocolLog(hdrTypGet(pPacket)); Log("\r\n");        
    }
    else
    {
        Log("ETH   header ");
        EthProtocolLog(hdrTypGet(pPacket));
        Log(" ");
        MacLog(hdrSrcPtr(pPacket));
        Log(" >>> ");
        MacLog(hdrDstPtr(pPacket));
        Log("\r\n");
    }
}
static char* tracePacket;
static void trace() { LogHeader(tracePacket); }
int EthHandlePacket(char* pPacketRx, int sizeRx, char* pPacketTx, int* pSizeTx)
{	
    int lastRestartPoint = RestartPoint;
    RestartPoint = FAULT_POINT_EthHandlePacket;
    
    tracePacket = pPacketRx;
    
    char* pDataRx    = pPacketRx + HEADER_LENGTH;
    char* pDataTx    = pPacketTx + HEADER_LENGTH;
    int dataLengthRx =    sizeRx - HEADER_LENGTH;
    int dataLengthTx =  *pSizeTx - HEADER_LENGTH;
    if (dataLengthTx > MTU) dataLengthTx = MTU; //Limit the transmitted length to the maximum ethernet frame payload length
        
    if (!MacAccept(hdrDstPtr(pPacketRx)))
    {
        RestartPoint = lastRestartPoint;
        return DO_NOTHING;
    }
    
    EthProtocol = hdrTypGet(pPacketRx);
    if (EthProtocol < 1500)
    {
        RestartPoint = lastRestartPoint;
        return DO_NOTHING; //drop 802.3 messages
    }

    NetTraceHostCheckMac(hdrSrcPtr(pPacketRx));

    int   action = DO_NOTHING;
    EthMacRemote = hdrSrcPtr(pPacketRx);
    switch (EthProtocol)
    {
        case ETH_ARP:  action = ArpHandleReceivedPacket(trace, pDataRx, dataLengthRx, pDataTx, &dataLengthTx);               break;
        case ETH_IPV4: action = Ip4HandleReceivedPacket(trace, pDataRx, dataLengthRx, pDataTx, &dataLengthTx, EthMacRemote); break;
        case ETH_IPV6: action = Ip6HandleReceivedPacket(trace, pDataRx, dataLengthRx, pDataTx, &dataLengthTx, EthMacRemote); break;
        case 0x6970: break; //Drop Sonos group membership packet
        case 0x7374: break; //Drop Sky Q packet
        case 0x7475: break; //Drop Sky Q packet
        case 0x7380: break; //Drop Sky Q packet
        case 0x8100: break; //Drop Sky Q VLAN 802.1Q packet
        case 0x887b: break; //Drop Sky Q packet
        default:
            LogTimeF("ETH protocol %d not handled", EthProtocol);
            break;
    }
    if (!action)
    {
        RestartPoint = lastRestartPoint;
        return DO_NOTHING;
    }
        
    MacMakeFromDest(ActionGetDestPart(action), EthProtocol, EthMacRemote);
    MacCopy(hdrSrcPtr(pPacketTx), MacLocal);
    MacCopy(hdrDstPtr(pPacketTx), EthMacRemote);
    hdrTypSet(pPacketTx, EthProtocol);
    
    *pSizeTx = HEADER_LENGTH + dataLengthTx;
    
    if (ActionGetTracePart(action)) LogHeader(pPacketTx);
    
    RestartPoint = lastRestartPoint;
    return action;
}
int EthPollForPacketToSend(char* pPacket, int* pSize)
{
    char* pData    = pPacket + HEADER_LENGTH;
    int dataLength =  *pSize - HEADER_LENGTH;
    if (dataLength > MTU) dataLength = MTU; //Limit the transmitted length to the maximum ethernet frame payload length
    
    int action = DO_NOTHING;
    EthProtocol = 0;
    if (!action)
    {
        EthProtocol = ETH_ARP;
        action = ArpPollForPacketToSend(pData, &dataLength);
    }

    if (!action)
    {
        EthProtocol = ETH_IPV6;
        action = Ip6PollForPacketToSend(pData, &dataLength, hdrDstPtr(pPacket));
    }
    
    if (!action)
    {
        EthProtocol = ETH_IPV4;
        action = Ip4PollForPacketToSend(pData, &dataLength, hdrDstPtr(pPacket));
    }
    
    if (!action) return DO_NOTHING;
    
    MacMakeFromDest(ActionGetDestPart(action), EthProtocol, hdrDstPtr(pPacket));
    MacCopy(hdrSrcPtr(pPacket), MacLocal);
    hdrTypSet(pPacket, EthProtocol);
    
    *pSize = HEADER_LENGTH + dataLength;
    
    if (ActionGetTracePart(action)) LogHeader(pPacket);
    
    return action;
}

