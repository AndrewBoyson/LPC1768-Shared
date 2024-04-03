#include <stdint.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"
#include "net/ip4/ip4.h"
#include "net/udp/dhcp/dhcp.h"
#include "net/ip4/icmp/echo4.h"
#include "net/checksum.h"

#define ECHO_REPLY   0
#define UNREACHABLE  3
#define REDIRECT     5
#define ECHO_REQUEST 8

static char* hdrTypePtr    (char* pPacket) { return pPacket +  0; } //1
static char* hdrCodePtr    (char* pPacket) { return pPacket +  1; } //1
static char* hdrChecksumPtr(char* pPacket) { return pPacket +  2; } //2
#define HEADER_LENGTH 4

static uint8_t  hdrTypeGet    (char* pPacket) { return                     *hdrTypePtr    (pPacket);            }
static uint8_t  hdrCodeGet    (char* pPacket) { return                     *hdrCodePtr    (pPacket);            }
static uint16_t hdrChecksumGet(char* pPacket) { uint16_t r; NetDirect16(&r, hdrChecksumPtr(pPacket)); return r; } //Don't invert the checksum

static void     hdrTypeSet    (char* pPacket, uint8_t  value) {            *hdrTypePtr    (pPacket) = value;  }
static void     hdrCodeSet    (char* pPacket, uint8_t  value) {            *hdrCodePtr    (pPacket) = value;  }
static void     hdrChecksumSet(char* pPacket, uint16_t value) { NetDirect16(hdrChecksumPtr(pPacket), &value); } //Don't invert the checksum

static void logType(uint16_t type)
{
    switch (type)
    {
        case ECHO_REPLY:   Log ("Echo Reply"           ); break;
        case ECHO_REQUEST: Log ("Echo Request"         ); break;
        default:           LogF("Unknown type %d", type); break;
    }
}
static void logHeader(int size, char* pPacket)
{            
    if (NetTraceVerbose)
    {
        Log ("ICMP4 header\r\n");
        LogF("  Type           "); logType(hdrTypeGet    (pPacket)); Log("\r\n");
        LogF("  Code           %u\r\n",    hdrCodeGet    (pPacket));
        LogF("  Checksum (hex) %04hX\r\n", hdrChecksumGet(pPacket));
        LogF("  Calculated     %04hX\r\n", CheckSum(size, pPacket));
    }
    else
    {
        Log("ICMP4 header ");
        logType(hdrTypeGet(pPacket));        
        Log("\r\n");
    }
}
static int traceSize = 0;
static char* tracePacket;
static void (*pTraceBack)(void);
static void trace()
{
    pTraceBack();
    logHeader(traceSize, tracePacket);
}
int Icmp4HandleReceivedPacket(void (*traceback)(void), char* pPacketRx, int sizeRx, char* pPacketTx, int* pSizeTx, uint32_t* pSrcIp, uint32_t* pDstIp)
{
    pTraceBack = traceback;
    tracePacket = pPacketRx;
    traceSize = sizeRx;

    uint8_t type = hdrTypeGet(pPacketRx);
    uint8_t code = hdrCodeGet(pPacketRx);
    
    int dataLengthRx =           sizeRx - HEADER_LENGTH;
    int dataLengthTx =         *pSizeTx - HEADER_LENGTH;
    char* pPayloadRx = (char*)pPacketRx + HEADER_LENGTH;
    char* pPayloadTx = (char*)pPacketTx + HEADER_LENGTH;
    
    int action = DO_NOTHING;
    switch (type)
    {
        case ECHO_REQUEST:
            action = Echo4HandleRequest(trace, &type, &code, pPayloadRx, dataLengthRx, pPayloadTx, &dataLengthTx);
            break;
        case UNREACHABLE:
            return DO_NOTHING;
        case REDIRECT:
            return DO_NOTHING;
        default:
            LogTimeF("ICMP4 packet type %d unknown\r\n", type);
            return DO_NOTHING;
    }
    if (!action) return DO_NOTHING;
    
    *pDstIp = *pSrcIp;
    *pSrcIp = DhcpLocalIp;

    *pSizeTx = HEADER_LENGTH + dataLengthTx;
    
    hdrTypeSet    (pPacketTx, type    );
    hdrCodeSet    (pPacketTx, code    );
    hdrChecksumSet(pPacketTx, 0       );
    uint16_t checksum = CheckSum(*pSizeTx, pPacketTx);
    hdrChecksumSet(pPacketTx, checksum);
    
    if (ActionGetTracePart(action)) logHeader(*pSizeTx, pPacketTx);
    
    return action;
}
