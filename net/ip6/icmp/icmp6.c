#include <stdint.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"
#include "net/ip6/icmp/ndp/ns.h"
#include "net/ip6/icmp/ndp/ra.h"
#include "net/ip6/icmp/ndp/rs.h"
#include "net/ip.h"
#include "net/ip6/ip6addr.h"
#include "net/ip6/slaac.h"
#include "net/ip6/icmp/echo6.h"
#include "net/ip6/icmp/dest6.h"
#include "net/checksum.h"

static char* hdrPtrType    (char* pPacket) { return pPacket + 0; } //1
static char* hdrPtrCode    (char* pPacket) { return pPacket + 1; } //1
static char* hdrPtrChecksum(char* pPacket) { return pPacket + 2; } //2
static const int HEADER_LENGTH = 4;

static uint8_t  type;
static uint8_t  code;
static uint16_t checksum;
static uint16_t calculatedChecksum;

static void logType(uint8_t type)
{
    switch (type)
    {
        case   1: Log ("Destination unreacheable" ); break;
        case 128: Log ("Echo Request"             ); break;
        case 129: Log ("Echo Reply"               ); break;
        case 133: Log ("Router solicit"           ); break;
        case 134: Log ("Router advertisement"     ); break;
        case 135: Log ("Neighbour solicit"        ); break;
        case 136: Log ("Neighbour advertisement"  ); break;
        case 137: Log ("Redirect"                 ); break;
        default:  LogF("Unknown type %u", type    ); break;
    }
}
static uint16_t calculateChecksum(char* pSrcIp, char* pDstIp, int size, char* pPacket)
{   
    uint32_t sum = 0;
    uint32_t pro32 = ICMP6;
     sum = CheckSumAddDirect(sum,   16, pSrcIp );
     sum = CheckSumAddDirect(sum,   16, pDstIp );
     sum = CheckSumAddInvert(sum,    4, &size  );
     sum = CheckSumAddInvert(sum,    4, &pro32 );
    return CheckSumFinDirect(sum, size, pPacket);
}
static void logHeader()
{
    if (NetTraceVerbose)
    {
        Log ("ICMP6 header\r\n");
        Log ("  Type           "); logType(type); Log("\r\n");
        LogF("  Code           %u\r\n",    code);
        LogF("  Checksum (hex) %04hX\r\n", checksum);
        LogF("  Calculated     %04hX\r\n", calculatedChecksum);
    }
    else
    {
        Log ("ICMP6 header ");
        logType(type);
        Log("\r\n");
    }
}
static void readHeader(char* pSrcIp, char* pDstIp, char* pPacket, int size)
{
    type = *hdrPtrType(pPacket);
    code = *hdrPtrCode(pPacket);
    NetDirect16(&checksum, hdrPtrChecksum(pPacket));
    calculatedChecksum = calculateChecksum(pSrcIp, pDstIp, size, pPacket);
}
static void writeHeader(char* pPacket, int size, char* pSrcIp, char* pDstIp)
{
    *hdrPtrType(pPacket) = type;
    *hdrPtrCode(pPacket) = code;
    checksum = 0;
    NetDirect16(hdrPtrChecksum(pPacket), &checksum);
    checksum = calculateChecksum(pSrcIp, pDstIp, size, pPacket);
    NetDirect16(hdrPtrChecksum(pPacket), &checksum);
    calculatedChecksum = 0;
}
static void (*pTraceBack)(void);
static void trace()
{
    pTraceBack();
    logHeader();
}
int Icmp6HandleReceivedPacket(void (*traceback)(void), int scope, char* pPacketRx, int sizeRx, char* pPacketTx, int* pSizeTx, char* pSrcIp, char* pDstIp)
{
    pTraceBack = traceback;
        
    readHeader(pSrcIp, pDstIp, pPacketRx, sizeRx);
    
    int dataLengthRx =    sizeRx - HEADER_LENGTH;
    int dataLengthTx =  *pSizeTx - HEADER_LENGTH;
    char* pPayloadRx = pPacketRx + HEADER_LENGTH;
    char* pPayloadTx = pPacketTx + HEADER_LENGTH;

    int action = DO_NOTHING;
    switch (type)
    {
        case   1: //Destination unreacheable
            action = Dest6HandleRequest(trace, &type, &code);
            break;
        case 128: //Echo request - Ping
            action = Echo6HandleRequest(trace, &type, &code, pPayloadRx, dataLengthRx, pPayloadTx, &dataLengthTx);
            break;
        case 133: //Router solicit
            return DO_NOTHING; //We are not a router so quietly drop this
        case 134: //Router advertisement
            action = RaHandleReceivedAdvertisement(trace, pPayloadRx, &dataLengthRx);
            break;
        case 135: //Neighbour solicit
            action = NsHandleReceivedSolicitation(trace, &type, &code, pPayloadRx, dataLengthRx, pPayloadTx, &dataLengthTx);
            break;
        case 136: //Neighbour advertisement
            action = NsHandleReceivedAdvertisement(trace, pPayloadRx, &dataLengthRx);
            break;
        case 137: //Redirect
            return DO_NOTHING; //Not sure yet how to handle these so ignore
        default:
            LogTimeF("ICMP6 unknown packet type %d\r\n", type);
            return DO_NOTHING;
    }
    if (!action) return DO_NOTHING;
    
    Ip6AddrCopy(pDstIp, pSrcIp);
    SlaacAddressFromScope(scope,                     pSrcIp);
      Ip6AddrFromDest (ActionGetDestPart(action), pDstIp);

    *pSizeTx = HEADER_LENGTH + dataLengthTx;
    
    writeHeader(pPacketTx, *pSizeTx, pSrcIp, pDstIp);
    
    if (ActionGetTracePart(action)) logHeader();
    
    return action;
}
int Icmp6PollForPacketToSend(char* pPacket, int* pSize, char* pSrcIp, char* pDstIp)
{    
    char* pData    = pPacket + HEADER_LENGTH;
    int dataLength =  *pSize - HEADER_LENGTH;
    int action  = DO_NOTHING;
    if (!action) action = RsGetWaitingSolicitation(pData, &dataLength, &type, &code);
    if (!action) action = NsGetWaitingSolicitation(pData, &dataLength, &type, &code);
    if (!action) return DO_NOTHING;

    int scope = SCOPE_LINK_LOCAL;
    SlaacAddressFromScope(scope,                     pSrcIp);
      Ip6AddrFromDest (ActionGetDestPart(action), pDstIp);

    *pSize = HEADER_LENGTH + dataLength;
    
    writeHeader(pPacket, *pSize, pSrcIp, pDstIp);
    
    if (ActionGetTracePart(action)) logHeader();
    
    return action;

}
