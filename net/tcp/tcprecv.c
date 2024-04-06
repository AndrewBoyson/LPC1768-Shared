#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"
#include "tcp.h"
#include "tcphdr.h"
#include "tcpsend.h"
#include "tcb.h"
#include "net/ip4/ip4.h"
#include "net/udp/dhcp/dhcp.h"
#include "httpv/httpv.h"
#include "https/https.h"
#include "lpc1768/led.h"
#include "lpc1768/mstimer/mstimer.h"


static void logTrace(void (*traceback)(void), char* fmt, ...)
{
    if (TcpTrace)
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTimeF("TCP port %hu - ", TcpHdrSrcPort);
        va_list argptr;
        va_start(argptr, fmt);
        LogV(fmt, argptr);
        va_end(argptr);
        Log("\r\n");
        if (NetTraceStack) traceback();
    }
}
static void handleSyn(void *pPacket, int ipType, int remArIndex, int locMss, struct tcb* pTcb)
{
    //Get the MSS to use for sends - it is the lower of the MSS advertised by the remote host and our local MSS
    int remMss = TcpHdrMssGet();
    pTcb->remMss = remMss ? remMss : 536; //default MSS for IPv4 [576 - 20(TCP) - 20(IP)];
    if (pTcb->remMss > locMss) pTcb->remMss = locMss;
    
    pTcb->timeSendsBeingAcked = MsTimerCount;
    pTcb->countSendsNotAcked  = 0;
    pTcb->rcvdFin             = false;
    pTcb->sentFin             = false;
    pTcb->remIsn              = TcpHdrSeqNum;
    pTcb->locIsn              = TcbGetIsn();
    pTcb->bytesRcvdFromRem    = 0;
    pTcb->bytesAckdByRem      = 0;
    pTcb->bytesAckdToRem      = 0;
    pTcb->bytesSentToRem      = 0;
    switch (pTcb->locPort) //Reset the application
    {
        case  80: HttpvReset(TcbGetId(pTcb)); break;
        case 443: HttpsReset(TcbGetId(pTcb)); break;
    }
}
static void handleReceivedData(void* pPacket, int dataLength, uint32_t position, struct tcb* pTcb)
{
    pTcb->window = TcpHdrWindow;
    uint8_t* pData = (uint8_t*)pPacket + TcpHdrSizeGet();
    switch (pTcb->locPort)
    {
        case  80: HttpvRequest(TcbGetId(pTcb), dataLength, pData, position); break;
        case 443: HttpsRequest(TcbGetId(pTcb), dataLength, pData, position); break;
    }
}
static int sendResetFromPacket(int* pSizeTx, void* pPacketTx, int ipType, int remArIndex, int locIpScope, int seqLengthRcvd)
{
    /*RFC793 p36 If the connection does not exist (CLOSED) then a reset is sent
    in response to any incoming segment except another reset.
    If the incoming segment has an ACK field, the reset takes its sequence number from the ACK field of the segment,
    otherwise                                 the reset has sequence number zero
    and
    the ACK field is set to the sum of the sequence number and segment length of the incoming segment.
    The connection remains in the CLOSED state.
    In TcpSendReset TcpHdrAckNum = pTcb->bytesAckdToRem + pTcb->remIsn;  //Set up the acknowledgement field ready to send
                    TcpHdrSeqNum = pTcb->bytesSentToRem + pTcb->locIsn;  //Set up the start of the message before adding the bytes sent
    */
    
    struct tcb tcb;
    struct tcb* pTcb = &tcb;
    pTcb->timeLastRcvd     = MsTimerCount;
    pTcb->remArIndex       = remArIndex;
    pTcb->ipType           = ipType;
    pTcb->locIpScope       = locIpScope;
    pTcb->remPort          = TcpHdrSrcPort;
    pTcb->locPort          = TcpHdrDstPort;
    pTcb->window           = TcpHdrWindow;
    pTcb->state            = TCB_EMPTY;

    pTcb->timeSendsBeingAcked = MsTimerCount;
    pTcb->countSendsNotAcked  = 0;
    pTcb->rcvdFin             = false;
    pTcb->sentFin             = false;
    pTcb->remIsn              = TcpHdrSeqNum + seqLengthRcvd; //Ack number
    pTcb->locIsn              = TcpHdrACK ? TcpHdrAckNum : 0; //Seq number
    pTcb->bytesRcvdFromRem    = 0;
    pTcb->bytesAckdByRem      = 0;
    pTcb->bytesAckdToRem      = 0;
    pTcb->bytesSentToRem      = 0;
    
    return TcpSendReset(pSizeTx, pPacketTx, pTcb);
}

int TcpHandleReceivedPacket(void (*traceback)(void), int sizeRx, void* pPacketRx, int* pSizeTx, void* pPacketTx, int ipType, int remArIndex, int locIpScope)
{
    
    int action = DO_NOTHING;
    bool traceRequested = false;
    
    TcpHdrReadFromPacket(pPacketRx);
    
    if (remArIndex < 0)
    {
        logTrace(traceback, "invalid remote AR index %d -> ignored packet", remArIndex);
        return DO_NOTHING;
    }
    
    int dataLength =   sizeRx - TcpHdrSizeGet();
    int locMss     = *pSizeTx - TcpHdrSizeGet();
    
    //Calculate the sequence length of the received packet
    int seqLengthRcvd = 0;
    if (TcpHdrSYN) seqLengthRcvd += 1;          //Add one to acknowledge the SYN
                   seqLengthRcvd += dataLength; //Add the number of bytes received
    if (TcpHdrFIN) seqLengthRcvd += 1;          //Add one to acknowledge the FIN
    
    //Filter out unwanted links
    switch (TcpHdrDstPort)
    {
        case 80:
            if (HttpvGetTrace())
            {
                if (NetTraceNewLine) Log("\r\n");
                LogTime("HTTP server request\r\n");
                traceRequested = true;
            }
            break;
            
        case 443:
            if (HttpsGetTrace())
            {
                if (NetTraceNewLine) Log("\r\n");
                LogTimeF("HTTPS server request of %d bytes\r\n", dataLength);
                traceRequested = true;
            }
            break;
            
        default: //Send reset if unknown port
            logTrace(traceback, "unhandled local port %hu -> sent reset", TcpHdrDstPort);
            action = sendResetFromPacket(pSizeTx, pPacketTx, ipType, remArIndex, locIpScope, seqLengthRcvd);
            return action;
    }
    
    //Get the Transmission Control Block
    struct tcb* pTcb = TcbGetExisting(ipType, remArIndex, locIpScope, TcpHdrSrcPort, TcpHdrDstPort);
    if (!pTcb) pTcb = TcbGetEmpty();
    if (!pTcb) //send reset if no more tcbs are available
    {
        logTrace(traceback, "no more tcbs available -> sent reset");
        action = sendResetFromPacket(pSizeTx, pPacketTx, ipType, remArIndex, locIpScope, seqLengthRcvd);
        return action;
    }
    pTcb->timeLastRcvd     = MsTimerCount;
    pTcb->remArIndex       = remArIndex;
    pTcb->ipType           = ipType;
    pTcb->locIpScope       = locIpScope;
    pTcb->remPort          = TcpHdrSrcPort;
    pTcb->locPort          = TcpHdrDstPort;
    pTcb->window           = TcpHdrWindow;
    
    //Handle request to reset
    if (TcpHdrRST)
    {
        if (pTcb->state)
        {
            logTrace(traceback, "received reset -> reaped TCB");
            pTcb->state = TCB_EMPTY;
        }
        return DO_NOTHING;        //Don't reply
    }
    
    //Handle request to synchronise
    if (TcpHdrSYN)
    {
        if (pTcb->state)
        {
            logTrace(traceback, "received a SYN on an open connection -> sent reset");
            pTcb->state = TCB_EMPTY;
            action = TcpSendReset(pSizeTx, pPacketTx, pTcb);
            return action;
        }
        else
        {
            handleSyn(pPacketRx, ipType, remArIndex, locMss, pTcb);
        }
    }
    
    //Handle non SYN packet on an empty connection
    if (!TcpHdrSYN && !pTcb->state)
    {

        logTrace(traceback, "non SYN packet received on a closed connection -> sent reset");
        pTcb->state = TCB_EMPTY;
        action = sendResetFromPacket(pSizeTx, pPacketTx, ipType, remArIndex, locIpScope, seqLengthRcvd);
        return action;
    }
    
    //Check if the acks of bytes sent has progressed and reset the timer
    uint32_t ackRcvdFromRem = TcpHdrACK ? TcpHdrAckNum - pTcb->locIsn : 0;
    if (ackRcvdFromRem > pTcb->bytesAckdByRem)
    {
        pTcb->timeSendsBeingAcked = MsTimerCount;
        pTcb->countSendsNotAcked  = 0;
    }

    //Record the number of bytes acked by the remote host
    pTcb->bytesAckdByRem = ackRcvdFromRem;

    /* If the connection is in a synchronized state
    any unacceptable segment (out of window sequence number or
    unacceptible acknowledgment number) must elicit only an empty
    acknowledgment segment containing the current send-sequence number
    and an acknowledgment indicating the next sequence number expected
    to be received, and the connection remains in the same state.*/
    uint32_t seqRcvdFromRem = TcpHdrSeqNum - pTcb->remIsn;
    if (seqRcvdFromRem != pTcb->bytesAckdToRem)
    {
        //Only warn non keep-alives
        if (seqRcvdFromRem != 0 || pTcb->bytesAckdToRem != 1)
        {
            logTrace(traceback, "seq rcvd is %d and last seq ackd was %d -> resent last ACK", seqRcvdFromRem, pTcb->bytesAckdToRem);
        }
        action = TcpResendLastAck(pSizeTx, pPacketTx, pTcb);
        return action;
    }
    //Ignore data before established
    if (pTcb->state != TCB_ESTABLISHED && dataLength)
    {
        logTrace(traceback, "data received before connection established -> sent reset");
        pTcb->state = TCB_EMPTY;
        action = TcpSendReset(pSizeTx, pPacketTx, pTcb);
        return action;
    }
    
    //Handle FIN
    if (TcpHdrFIN) pTcb->rcvdFin = true; //When reply is all sent only a passive close is needed
    
    //From now on there are no errors so display traceback if requested
    if (traceRequested && NetTraceStack) traceback();
    
    //Record the number of bytes received from the remote host
    pTcb->bytesRcvdFromRem += seqLengthRcvd;

    switch (pTcb->state) //This is the state of the connection BEFORE this packet arrived
    {
        case TCB_EMPTY:
            pTcb->state = TCB_SYN_RECEIVED;
            break;
            
        case TCB_SYN_RECEIVED:
            pTcb->state = TCB_ESTABLISHED;
            break;
            
        case TCB_ESTABLISHED:
            if (dataLength)
            {
                handleReceivedData(pPacketRx, dataLength, seqRcvdFromRem - 1, pTcb);
            }
            if (pTcb->sentFin)
            {
                pTcb->state = pTcb->rcvdFin ? TCB_EMPTY : TCB_CLOSE_FIN_WAIT;
            }
            break;
            
        case TCB_CLOSE_FIN_WAIT: //End of active close
            if (TcpHdrFIN)
            {
                pTcb->state = TCB_EMPTY;//Ignore ACK to our FIN. Wait for FIN then close.
            }
            break;
            
    }
    
    action = TcpSend(pSizeTx, pPacketTx, pTcb);

    return action;
}
