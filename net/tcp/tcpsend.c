#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"
#include "tcp.h"
#include "tcphdr.h"
#include "tcb.h"
#include "net/ip4/ip4.h"
#include "net/udp/dhcp/dhcp.h"
#include "httpv/httpv.h"
#include "lpc1768/led.h"
#include "tcpsend.h"
#include "lpc1768/mstimer/mstimer.h"

#define TIMEOUT_RETRANSMISSION_MS  700
#define MAX_RETRANSMISSIONS          5
#define TIMEOUT_BROKEN_LINK_MS  600000

static void logTrace(uint16_t remPort, char* fmt, ...)
{
    if (TcpTrace)
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTimeF("TCP port %hu - ", remPort);
        va_list argptr;
        va_start(argptr, fmt);
        LogV(fmt, argptr);
        va_end(argptr);
        Log("\r\n");
    }
}

static bool doTrace(uint16_t port)
{
    switch (port)
    {
        case  80: return HttpvGetTrace();
        default:  return false;
    }    
}

static bool addAppData(int *pDataLength, void* pPacket, int connection, uint16_t port, uint32_t windowPositionInStream, int windowSize, bool clientFinished)
{
    uint8_t* pWindow = (uint8_t*)pPacket + TcpHdrSizeGet();
    bool finished = false;
    *pDataLength = windowSize;
    switch (port)
    {
        case  80: finished = HttpvResponse(connection, clientFinished, pDataLength, pWindow, windowPositionInStream); break;
    }
    return finished;
}
static int preparePacket(void* pPacket, struct tcb* pTcb, int dataLength, int* pSize)
{
    //Set the acknowledge flag
    TcpHdrACK = true;
    
    //Swap the ports for the reply
    TcpHdrSrcPort = pTcb->locPort;
    TcpHdrDstPort = pTcb->remPort;
    
    //Specify the receive window size to not throttle
    TcpHdrWindow = 4000;
    
    //Write the header
    TcpHdrWriteToPacket(pPacket);
    
    //Calculate the size of the reply
    *pSize = TcpHdrSizeGet() + dataLength;
    
    return ActionMakeFromDestAndTrace(UNICAST, doTrace(pTcb->locPort) && NetTraceStack);
}
int TcpSend(int* pSize, void* pPacket, struct tcb* pTcb)
{
    int dataLength = 0;
    TcpHdrMakeEmpty();
    int locMss = *pSize - TcpHdrSizeGet();
    switch (pTcb->state)
    {
        case TCB_SYN_RECEIVED:
            if (pTcb->bytesSentToRem == 0)
            {
                TcpHdrMssSet(locMss);
                TcpHdrSYN = true;
            }
            break;
            
        case TCB_ESTABLISHED:
            if (!pTcb->sentFin)
            {
                if (pTcb->bytesSentToRem - pTcb->bytesAckdByRem < pTcb->window)
                {
                    bool finished = addAppData(&dataLength, pPacket, TcbGetId(pTcb), pTcb->locPort, pTcb->bytesSentToRem - 1, pTcb->remMss, pTcb->rcvdFin);
                    if (finished)
                    {
                        TcpHdrFIN     = true;
                        pTcb->sentFin = true;
                    }
                }
            }
            break;
    }

    //Handle the acknowledgement of received bytes
    bool rcvdSeqHasAdvanced = pTcb->bytesRcvdFromRem > pTcb->bytesAckdToRem;
    pTcb->bytesAckdToRem = pTcb->bytesRcvdFromRem;
    TcpHdrAckNum  = pTcb->bytesAckdToRem + pTcb->remIsn;  //Set up the acknowledgement field ready to send
    
    //Specify the start of the data being sent
    uint32_t seqToSend = pTcb->bytesSentToRem;
    TcpHdrSeqNum  = seqToSend            + pTcb->locIsn;  //Set up the start of the message before adding the bytes sent

    //Record the number of bytes sent
    uint32_t bytesToSend = 0;
    if (TcpHdrSYN) bytesToSend += 1;            //Add one to acknowledge the SYN
                   bytesToSend += dataLength;   //Add the number of bytes received
    if (TcpHdrFIN) bytesToSend += 1;            //Add one to acknowledge the FIN

    pTcb->bytesSentToRem += bytesToSend;
    
    //Only send a packet if have bytes or an acknowledgement to send
    if (!rcvdSeqHasAdvanced && !bytesToSend) return DO_NOTHING;
        
    return preparePacket(pPacket, pTcb, dataLength, pSize);
}
int TcpResendLastUnAcked(int* pSize, void *pPacket, struct tcb* pTcb)
{
    int dataLength = 0;
    TcpHdrMakeEmpty();
    int locMss = *pSize - TcpHdrSizeGet();
    uint32_t seqNum = pTcb->bytesAckdByRem;
    switch (pTcb->state)
    {
        case TCB_SYN_RECEIVED:
            TcpHdrMssSet(locMss);
            TcpHdrSYN = true;
            break;
            
        case TCB_ESTABLISHED:
        case TCB_CLOSE_FIN_WAIT:
            {
                bool finished = addAppData(&dataLength, pPacket, TcbGetId(pTcb), pTcb->locPort, seqNum - 1, pTcb->remMss, pTcb->rcvdFin);
                if (finished)
                {
                    TcpHdrFIN     = true;
                    pTcb->sentFin = true;
                }
                break;
            }
    }

    TcpHdrAckNum = pTcb->bytesAckdToRem + pTcb->remIsn;  //Set up the acknowledgement field ready to send
    TcpHdrSeqNum = seqNum               + pTcb->locIsn;  //Set up the start of the message before adding the bytes sent
    
    return preparePacket(pPacket, pTcb, dataLength, pSize);
}
int TcpResendLastAck(int* pSize, void *pPacket, struct tcb* pTcb)
{
    int dataLength = 0;
    TcpHdrMakeEmpty();
    TcpHdrAckNum = pTcb->bytesAckdToRem + pTcb->remIsn;  //Set up the acknowledgement field ready to send
    TcpHdrSeqNum = pTcb->bytesSentToRem + pTcb->locIsn;  //Set up the start of the message before adding the bytes sent
    
    return preparePacket(pPacket, pTcb, dataLength, pSize);
}
int TcpSendReset(int* pSize, void *pPacket, struct tcb* pTcb)
{
    int dataLength = 0;
    TcpHdrMakeEmpty();
    TcpHdrAckNum = pTcb->bytesAckdToRem + pTcb->remIsn;  //Set up the acknowledgement field ready to send
    TcpHdrSeqNum = pTcb->bytesSentToRem + pTcb->locIsn;  //Set up the start of the message before adding the bytes sent
    
    TcpHdrRST = true;
    
    return preparePacket(pPacket, pTcb, dataLength, pSize);
}
int TcpSendClose(int* pSize, void *pPacket, struct tcb* pTcb)
{
    int dataLength = 0;
    TcpHdrMakeEmpty();
    TcpHdrFIN     = true;
    pTcb->sentFin = true;
    pTcb->bytesSentToRem += 1; //For the FIN
    TcpHdrAckNum = pTcb->bytesAckdToRem + pTcb->remIsn;  //Set up the acknowledgement field ready to send
    TcpHdrSeqNum = pTcb->bytesSentToRem + pTcb->locIsn;  //Set up the start of the message before adding the bytes sent
    
    return preparePacket(pPacket, pTcb, dataLength, pSize);
}

int TcpPollForPacketToSend(int* pSize, void* pPacket, int ipType, int* pRemArIndex, int* pLocIpScope)
{
    //Loops around the TCBs, moving on if empty but staying if not the right type
    static struct tcb* pTcb = NULL; //Passing a pointer containing NULL to TcbGetNext causes it to return the first TCB
    static bool stay = false;
    if (!stay) pTcb = TcbGetNext(pTcb);
    stay = false;
    if (!pTcb->state) return DO_NOTHING;
    if (pTcb->ipType != ipType)
    {
        stay = true;
        return DO_NOTHING;
    }

    //Return the remote AR index and the local IP scope
    if (pRemArIndex) *pRemArIndex = pTcb->remArIndex;
    if (pLocIpScope) *pLocIpScope = pTcb->locIpScope;
            
    //Reap broken links 
    if (MsTimerRelative(pTcb->timeLastRcvd, TIMEOUT_BROKEN_LINK_MS))
    {
        logTrace(pTcb->remPort, "broken link -> reaping TCB");
        pTcb->state = TCB_EMPTY;
        return DO_NOTHING;
    }
    
    //Reset the RTO if all bytes are acknowledged
    if (pTcb->bytesSentToRem == pTcb->bytesAckdByRem)
    {
        pTcb->timeSendsBeingAcked = MsTimerCount;
        pTcb->countSendsNotAcked  = 0;
    }
    
    //Check if have unacknowledged send bytes after the RTO
    if (MsTimerRelative(pTcb->timeSendsBeingAcked, TIMEOUT_RETRANSMISSION_MS))
    {
        pTcb->countSendsNotAcked++;
        if (pTcb->countSendsNotAcked > MAX_RETRANSMISSIONS)
        {
            logTrace(pTcb->remPort, "reached maximum retransmissions -> reaping TCB");
            pTcb->state = TCB_EMPTY;
            return DO_NOTHING;
        }
        else
        {
            logTrace(pTcb->remPort, "only had ack of %lu while sent %lu -> resending", pTcb->bytesAckdByRem, pTcb->bytesSentToRem);
            pTcb->timeSendsBeingAcked = MsTimerCount;
            return TcpResendLastUnAcked(pSize, pPacket, pTcb);
        }
    }
    else
    {
        //If haven't had to do anything else then do a normal send
        return TcpSend(pSize, pPacket, pTcb);
    }
}

