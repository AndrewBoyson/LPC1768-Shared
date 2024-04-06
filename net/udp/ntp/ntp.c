#include <stdint.h>
#include <stdbool.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"
#include "net/udp/udp.h"
#include "ntp.h"
#include "ntpclient.h"
#include "ntpserver.h"
#include "ntphdr.h"
#include "clock/clk/clktime.h"

bool NtpTrace = false;

void NtpInit()
{
   NtpClientInit();
}

void NtpLogHeader(char* pPacket)
{
    if (NetTraceVerbose) Log ("NTP header\r\n  ");
    else                 Log ("NTP   header: ");
    
    LogF("Mode %d",         NtpHdrGetMode(pPacket));
    LogF(", Version %d",    NtpHdrGetVersion(pPacket));
    LogF(", LI %d",         NtpHdrGetLI(pPacket));
    LogF(", Stratum %d",    NtpHdrGetStratum(pPacket));
    LogF(", Poll %d",       NtpHdrGetPoll(pPacket));
    LogF(", Precision %d",  NtpHdrGetPrecision(pPacket));
    LogF(", Root delay %d", NtpHdrGetRootDelay(pPacket));
    LogF(", Dispersion %d", NtpHdrGetDispersion(pPacket));
    Log (", Ident ");
    for (int i = 0; i < 4; i++) if (NtpHdrPtrRefIdentifier(pPacket)[i]) LogChar(NtpHdrPtrRefIdentifier(pPacket)[i]);
    Log ("\r\n");
    
    if (NetTraceVerbose)
    {
        LogF("  REF %llu\r\n",  NtpHdrGetRefTimeStamp(pPacket));
        LogF("  ORI %llu\r\n",  NtpHdrGetOriTimeStamp(pPacket));
        LogF("  REC %llu\r\n",  NtpHdrGetRecTimeStamp(pPacket));
        LogF("  TRA %llu\r\n",  NtpHdrGetTraTimeStamp(pPacket));
    }
}

int NtpHandlePacketReceived(void (*traceback)(void), int sizeRx, char* pPacketRx, int* pSizeTx, char* pPacketTx)
{
    if (sizeRx != NTP_HEADER_LENGTH)
    {
        LogTimeF("\r\nNTP packet wrong size %d\r\n", sizeRx);
        return DO_NOTHING;
    }
    
    int dest = DO_NOTHING;
    switch (NtpHdrGetMode(pPacketRx))
    {
        case NTP_CLIENT: dest = NtpServerRequest(traceback, pPacketRx, pPacketTx);        break;
        case NTP_SERVER:        NtpClientReply  (traceback, pPacketRx);                   break;
        default:         LogTimeF("\r\nNTP packet unknown mode %d\r\n", NtpHdrGetMode(pPacketRx)); break;
    }
    
    if (dest == DO_NOTHING)
    {
        return DO_NOTHING;
    }
    else
    {
        *pSizeTx = NTP_HEADER_LENGTH;
        return ActionMakeFromDestAndTrace(dest, NtpTrace && NetTraceStack);
    }
}
int NtpPollForPacketToSend(int type, char* pPacket, int* pSize)
{    
    int dest = NtpClientQueryPoll(type, pPacket, pSize);
    if (!dest) return DO_NOTHING;
    
    if (NtpTrace)
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTimeF("Sending NTP request\r\n");
        NtpLogHeader(pPacket);
    }
    return ActionMakeFromDestAndTrace(dest, NtpTrace && NetTraceStack);


}

