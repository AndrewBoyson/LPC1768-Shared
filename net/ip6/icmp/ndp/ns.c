#include <stdint.h>
#include <stdbool.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"
#include "net/ip6/ip6.h"
#include "net/ip6/slaac.h"
#include "net/ip6/ip6addr.h"
#include "net/eth/mac.h"
#include "net/resolve/ar6.h"
#include "net/resolve/nr.h"
#include "ndp.h"

bool NsTraceRecvSol = false;
bool NsTraceRecvAdv = false;
bool NsTraceSendSol = false;

char NsAddressToResolve[16];
bool NsResolveRequestFlag = false;

static char* hdrPtrReserved(char* pPacket) { return pPacket +  0; } // 4
static char* hdrPtrTarget  (char* pPacket) { return pPacket +  4; } //16
const int HEADER_LENGTH = 20;
static void  hdrSetReserved(char* pPacket, uint32_t value) { NetInvert32(hdrPtrReserved(pPacket), &value); }

static void logHeader(char* pPacket, int size)
{
    char* pData = pPacket + HEADER_LENGTH;
    int dataLength = size - HEADER_LENGTH;
    
    if (NetTraceVerbose)
    {
        Log("NS header\r\n");
        LogF("  Size        %d\r\n", size);
        LogF("  Target      "); Ip6AddrLog(hdrPtrTarget(pPacket)); Log("\r\n");
        NdpLogOptionsVerbose(pData, dataLength);
    }
    else
    {
        Log("NS    header ");
        Ip6AddrLog(hdrPtrTarget(pPacket));
        NdpLogOptionsQuiet(pData, dataLength); 
        Log("\r\n");
    }
}
static char* pTraceHeader;
static int traceSize;
static void (*pTraceBack)(void);
static void trace()
{
    pTraceBack();
    logHeader(pTraceHeader, traceSize);
}
int NsHandleReceivedSolicitation(void (*traceback)(void), uint8_t* pType, uint8_t* pCode, char* pPacketRx, int sizeRx, char* pPacketTx, int* pSizeTx)
{
    pTraceBack = traceback;
    
    pTraceHeader = pPacketRx;
    traceSize = sizeRx;
    
    //Check it is us
    if (!SlaacScope(hdrPtrTarget(pPacketRx))) return DO_NOTHING;

    if (NsTraceRecvSol)
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTimeF("NDP received neighbour solicit\r\n");
        if (NetTraceStack) traceback();
        logHeader(pPacketRx, sizeRx);
    }
    
    //Send advertisement
    *pType = 136;
    *pCode = 0;
    hdrSetReserved(pPacketTx, 0x60000000); //R=0 (not a router); S=1 (solicited); O=1 (override)
    Ip6AddrCopy(hdrPtrTarget(pPacketTx), hdrPtrTarget(pPacketRx)); //Target does not change
    
    //Add target MAC
    char* pDataTx = pPacketTx + HEADER_LENGTH;
    char* p = pDataTx;
    p += NdpAddOptionTargetMac(p, MacLocal);

    *pSizeTx = HEADER_LENGTH + p - pDataTx;
    
    if (NsTraceRecvSol) logHeader(pPacketTx, *pSizeTx);
    
    return ActionMakeFromDestAndTrace(UNICAST, NsTraceRecvSol && NetTraceStack);
}
int NsHandleReceivedAdvertisement(void (*traceback)(void), char* pPacket, int* pSize)
{
    pTraceBack = traceback;
    char* pData    = pPacket + HEADER_LENGTH;
    int dataLength = *pSize  - HEADER_LENGTH;
    
    if (NsTraceRecvAdv)
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTimeF("NDP received neighbour advertise\r\n");
        if (NetTraceStack) traceback();
        logHeader(pPacket, *pSize);
    }
    
    char tgtMac[6];
    NdpDecodeOptions(pData, dataLength, NULL, tgtMac);
    
    Ar6AddIpRecord(trace, tgtMac, hdrPtrTarget(pPacket));
    NrMakeRequestForNameFromAddress6(hdrPtrTarget(pPacket));
    
    return DO_NOTHING;
}

int NsGetWaitingSolicitation(char* pPacket, int* pSize, uint8_t* pType, uint8_t* pCode)
{  
    if (!NsResolveRequestFlag) return DO_NOTHING;
    NsResolveRequestFlag = false;

    *pType = 135; //Neighbour solicitation
    *pCode = 0;
        
    hdrSetReserved(pPacket, 0);
    Ip6AddrCopy(hdrPtrTarget(pPacket), NsAddressToResolve);
    
    char* pData = pPacket + HEADER_LENGTH;
    char* p = pData;
    p += NdpAddOptionSourceMac(p, MacLocal);
    
    *pSize = HEADER_LENGTH + p - pData;

    if (NsTraceSendSol)
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTimeF("NDP sent neighbour solicit\r\n");
        logHeader(pPacket, *pSize);
    }
    
    return ActionMakeFromDestAndTrace(MULTICAST_NODE, NsTraceSendSol && NetTraceStack);
    
}
