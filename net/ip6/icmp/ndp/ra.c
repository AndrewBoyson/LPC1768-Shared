#include <stdint.h>
#include <stdbool.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"
#include "net/ip6/ip6.h"
#include "net/eth/mac.h"
#include "net/ip6/slaac.h"
#include "ndp.h"

bool RaTrace = false;

static uint8_t  hdrGetHop      (char* pPacket) {                    return *(pPacket + 0);           }
static uint8_t  hdrGetMo       (char* pPacket) {                    return *(pPacket + 1);           }
static uint16_t hdrGetLifetime (char* pPacket) { uint16_t r; NetInvert16(&r, pPacket + 2); return r; }
static uint32_t hdrGetReachable(char* pPacket) { uint32_t r; NetInvert32(&r, pPacket + 4); return r; }
static uint32_t hdrGetRetrans  (char* pPacket) { uint32_t r; NetInvert32(&r, pPacket + 8); return r; }
static const int HEADER_LENGTH = 12;

void logHeader(char* pPacket, int dataLength)
{
    char* pData =  pPacket + HEADER_LENGTH;
    if (NetTraceVerbose)
    {
        Log("RA header\r\n");
        LogF("  Hop limit   %d\r\n", hdrGetHop      (pPacket));
        LogF("  M O         %x\r\n", hdrGetMo       (pPacket));
        LogF("  Lifetime    %d\r\n", hdrGetLifetime (pPacket));
        LogF("  Reachable   %d\r\n", hdrGetReachable(pPacket));
        LogF("  Retrans     %d\r\n", hdrGetRetrans  (pPacket));
        NdpLogOptionsVerbose(pData, dataLength);
    }
    else
    {
        Log("RA    header");
        NdpLogOptionsQuiet(pData, dataLength);
        Log("\r\n");
    }
}
int RaHandleReceivedAdvertisement(void (*traceback)(void), char* pPacket, int* pSize)
{
    char*    pData =  pPacket + HEADER_LENGTH;
    int dataLength = *pSize   - HEADER_LENGTH;
    
    NdpHopLimit             = hdrGetHop     (pPacket);
    NdpManagedConfiguration = hdrGetMo      (pPacket) & 0x80;
    NdpOtherConfiguration   = hdrGetMo      (pPacket) & 0x40;
    NdpSetLease              (hdrGetLifetime(pPacket)); //This resets the NdpElapsedTimer
    
    if (RaTrace)
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTimeF("NDP received router advertise\r\n");
        if (NetTraceStack) traceback();
        logHeader(pPacket, dataLength);
    }
    NdpDecodeOptions(pData, dataLength, NdpRouterMac, NULL);
    
    return DO_NOTHING;

}
