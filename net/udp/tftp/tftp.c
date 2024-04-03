#include <stdint.h>
#include <stdbool.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"
#include "net/eth/eth.h"
#include "net/udp/dhcp/dhcp.h"
#include "net/udp/udp.h"
#include "net/udp/dns/dns.h"
#include "net/udp/dns/dnslabel.h"
#include "net/udp/tftp/tftp.h"
#include "lpc1768/mstimer/mstimer.h"
#include "net/resolve/resolve.h"

bool TftpTrace = false;

#define WRITE_TIMEOUT_MS 7000 //Make this longer than the resolve time which is up to 3 seconds

#define TFTP_RRQ   1
#define TFTP_WRQ   2
#define TFTP_DATA  3
#define TFTP_ACK   4
#define TFTP_ERROR 5

static void logOp(char* p)
{
    if (*p)
    {
        LogF("Unknown op code %02x", *p); LogF("%02x", *++p);
        return;
    }
    p++;
    switch (*p)
    {
        case TFTP_RRQ:   Log ("RRQ")                         ; break;
        case TFTP_WRQ:   Log ("WRQ")                         ; break;
        case TFTP_DATA:  Log ("DATA")                        ; break;
        case TFTP_ACK:   Log ("ACK")                         ; break;
        case TFTP_ERROR: Log ("ERROR")                       ; break;
        default:         LogF("Unknown op code 00%02x", *p); break;
    }
}

static void logError(char* p)
{
    if (*p)
    {
        LogF("Unknown error code %02x", *p);
        LogF("%02x", *++p);
        return;
    }
    p++;
    switch (*p)
    {
        case 0:  Log ("Not defined, see error message."  ); break;
        case 1:  Log ("File not found."                  ); break;
        case 2:  Log ("Access violation."                ); break;
        case 3:  Log ("Disk full or allocation exceeded."); break;
        case 4:  Log ("Illegal TFTP operation."          ); break;
        case 5:  Log ("Unknown transfer ID."             ); break;
        case 6:  Log ("File already exists."             ); break;
        case 7:  Log ("No such user."                    ); break;
        default: LogF("Unknown error code 00%02x", *p    ); break;
    }
}
bool       TftpSendRequestsViaIp4 = false;
uint32_t   TftpServerIp4;
char       TftpServerIp6[16];
int        TftpWriteStatus = TFTP_WRITE_STATUS_UNAVAILABLE;
char       TftpServerName[DNS_MAX_LABEL_LENGTH+1];
char       TftpFileName  [DNS_MAX_LABEL_LENGTH+1];

int      (*TftpGetNextByteFunction)();

static void logHeader(int size, char* p)
{
    if (NetTraceVerbose)
    {
        Log ("TFTP header\r\n");
        Log ("  Op code "); logOp(p);         Log("\r\n");
        Log ("  Size    "); LogF("%d", size); Log("\r\n");
    }
    else
    {
        Log ("TFTP  header");
        Log (": Op "); logOp(p);
        LogF(", %d bytes", size);
        Log ("\r\n");
    }
}
static int sendBlock(uint16_t block, int* pSize, char* pHeader)
{
    /*2 bytes    2 bytes     n bytes
     ----------------------------------
    | Opcode |   Block #  |   Data     |
     ---------------------------------- */
     char* p = pHeader;
    *p++ = 0;
    *p++ = TFTP_DATA;
    *p++ = block >> 8;
    *p++ = block & 0xFF;
    
    int len = 0;
    while (len < 512)
    {
        int c = TftpGetNextByteFunction();
        if (c == -1) break;
        *p++ = c;
        len++;
    }
    if (len < 512) TftpWriteStatus = TFTP_WRITE_STATUS_NONE;
    *pSize = p - pHeader;
    return UNICAST_TFTP;
}
static void handleError(char* p)
{
     /*2 bytes   2 bytes      string    1 byte
     -----------------------------------------
    |  ERROR |  ErrorCode |   ErrMsg   |   0  |
     ----------------------------------------- */
    p += 2; //Skip the op code which we already know
    if (NetTraceNewLine) Log("\r\n");
    LogTime("TFTP error - ");
    logError(p); p += 2;
    Log(p);
    Log("\r\n");
    TftpWriteStatus = TFTP_WRITE_STATUS_NONE;
}
static int handleAck(char* pHeaderRx, int* pSizeTx, char* pHeaderTx)
{
    /* 2 bytes    2 bytes
     -----------------------
    |   ACK    |   Block #  |
     ----------------------- */
    char* p = pHeaderRx;
    p += 2; //Skip the op code which we already know
    uint16_t block = *p++;
    block <<= 8;
    block += *p++;
    
    return sendBlock(block + 1, pSizeTx, pHeaderTx);
}
static int sendRequest(int* pSize, char* pHeader)
{
    /*2 bytes    string   1 byte     string   1 byte
     -----------------------------------------------
    |   WRQ  |  Filename  |   0  |    Mode    |  0  |
     ----------------------------------------------- */
     char* p = pHeader;
    *p++ = 0;
    *p++ = TFTP_WRQ;
    char* pName = TftpFileName;
    while (*pName) *p++ = *pName++;
    *p++ = 0;
    const char* pMode = "octet";
    while (*pMode) *p++ = *pMode++;
    *p++ = 0;
    *pSize = p - pHeader;
    return UNICAST_TFTP;
}
int TftpHandlePacketReceived(void (*traceback)(void), int sizeRx, void * pPacketRx, int* pSizeTx, void* pPacketTx)
{
    char* pHeaderRx = (char*)pPacketRx;
    char* pHeaderTx = (char*)pPacketTx;
    
    char* p = pHeaderRx;
    
    if (*p)
    {
        LogTimeF("Expected high byte of op code to be zero not %u\r\n", *p);
        return DO_NOTHING;
    }
    if (TftpTrace)
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTimeF("TFTP received packet\r\n");
        if (NetTraceStack) traceback();
        logHeader(sizeRx, pHeaderRx);
    }
    p++;
    int dest = DO_NOTHING;
    switch (*p)
    {
        case TFTP_ACK:
            if (TftpWriteStatus == TFTP_WRITE_STATUS_NONE) return DO_NOTHING;
            dest = handleAck(pHeaderRx, pSizeTx, pHeaderTx);
            break;
        case TFTP_ERROR:
            handleError(pHeaderRx); 
            return DO_NOTHING;
        default:
            LogTimeF("\r\nTFTP packet unknown mode %d\r\n", *p);
            return DO_NOTHING;
    }

    if (TftpTrace) logHeader(*pSizeTx, pHeaderTx);

    return ActionMakeFromDestAndTrace(dest, TftpTrace && NetTraceStack);
}
static bool isTimedOut()
{
    static uint32_t writeStartMs = 0;
    
    if (TftpWriteStatus == TFTP_WRITE_STATUS_NONE || TftpWriteStatus == TFTP_WRITE_STATUS_UNAVAILABLE) writeStartMs = MsTimerCount;
    
    if (MsTimerRelative(writeStartMs, WRITE_TIMEOUT_MS))
    {
        LogTime("TFTP - write operation timed out so reset\r\n");
        writeStartMs = MsTimerCount;
        return true;
    }
    return false;
}
static bool isOkToGo()
{
    if (!TftpServerName[0])
    {
        LogTimeF("TftpPollForRequestToSend - A request to send a client message has been made but no server name has been specified\r\n");
        return false;
    }
    
    if (!TftpGetNextByteFunction)
    {
        LogTimeF("TftpPollForRequestToSend - A request to send a client message has been made but TFTP has not been plumbed into a file stream\r\n");
        return false;
    }
    return true;
}
int TftpPollForPacketToSend(int type, void* pPacket, int* pSize)
{
    if (type == ETH_IPV4)
    {
        if (!DhcpLocalIp)
        {
            TftpWriteStatus = TFTP_WRITE_STATUS_UNAVAILABLE;
        }
        else
        {
            if (TftpWriteStatus == TFTP_WRITE_STATUS_UNAVAILABLE) TftpWriteStatus = TFTP_WRITE_STATUS_NONE;
        }
    }
    if (isTimedOut()                                                 ) { TftpWriteStatus = TFTP_WRITE_STATUS_NONE; return DO_NOTHING; }
    if (TftpWriteStatus != TFTP_WRITE_STATUS_REQUEST                 ) {                                           return DO_NOTHING; }
    if (!isOkToGo()                                                  ) { TftpWriteStatus = TFTP_WRITE_STATUS_NONE; return DO_NOTHING; }
    if (!Resolve(TftpServerName, type, &TftpServerIp4, TftpServerIp6)) {                                           return DO_NOTHING; }

    //Have IP and MAC so send request
    TftpWriteStatus = TFTP_WRITE_STATUS_IN_PROGRESS;
    int dest = sendRequest(pSize, (char*)pPacket);
    if (TftpTrace)
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTimeF("TFTP Sending request\r\n");
        logHeader(*pSize, (char*)pPacket);
    }
    return ActionMakeFromDestAndTrace(dest, TftpTrace && NetTraceStack);
}