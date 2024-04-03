#include <stdint.h>
#include <stdbool.h>

#include "log/log.h"
#include "net/net.h"
#include "lpc1768/reset/restart.h"

static char* hdrPtrSrcPort   (char* pPacket) { return pPacket +  0; } //2
static char* hdrPtrDstPort   (char* pPacket) { return pPacket +  2; } //2
static char* hdrPtrSeqnum    (char* pPacket) { return pPacket +  4; } //4
static char* hdrPtrAcknum    (char* pPacket) { return pPacket +  8; } //4
static char* hdrPtrDataOffset(char* pPacket) { return pPacket + 12; } //1
static char* hdrPtrFlags     (char* pPacket) { return pPacket + 13; } //1
static char* hdrPtrWindow    (char* pPacket) { return pPacket + 14; } //2
static char* hdrPtrChecksum  (char* pPacket) { return pPacket + 16; } //2
static char* hdrPtrUrgent    (char* pPacket) { return pPacket + 18; } //2
static const int HEADER_LENGTH = 20;

void    TcpHdrSetChecksum (char* pPacket, uint16_t value) { NetDirect16(hdrPtrChecksum  (pPacket), &value);        } //The checksum is calculated inverted so don't re-invert here

//Header variables
uint16_t TcpHdrSrcPort;
uint16_t TcpHdrDstPort;
uint32_t TcpHdrSeqNum;
uint32_t TcpHdrAckNum;
bool     TcpHdrURG; //indicates that the Urgent pointer field is significant
bool     TcpHdrACK; //indicates that the Acknowledgment field is significant. All packets after the initial SYN packet sent by the client should have this flag set.
bool     TcpHdrPSH; //Push function. Asks to push the buffered data to the receiving application.
bool     TcpHdrRST; //Reset the connection
bool     TcpHdrSYN; //Synchronize sequence numbers. Only the first packet sent from each end should have this flag set. Some other flags and fields change meaning based on this flag, and some are only valid for when it is set, and others when it is clear.
bool     TcpHdrFIN; //No more data from sender
uint16_t TcpHdrWindow;
uint16_t TcpHdrChecksum;
uint16_t TcpHdrUrgent;

//Side effects
static int      headerSize;
int TcpHdrSizeGet() { return headerSize; }

static uint16_t mss = 0;
void TcpHdrMssSet(uint16_t value)
{
    mss = value;
    headerSize = HEADER_LENGTH;
    if (mss) headerSize += 4;
}
uint16_t TcpHdrMssGet() { return mss; }

static void logFlags()
{
    if (TcpHdrURG) Log(" URG");
    if (TcpHdrACK) Log(" ACK");
    if (TcpHdrPSH) Log(" PSH");
    if (TcpHdrRST) Log(" RST");
    if (TcpHdrSYN) Log(" SYN");
    if (TcpHdrFIN) Log(" FIN");
}
void TcpHdrLog(uint16_t calculatedChecksum)
{
    if (NetTraceVerbose)
    {
        Log("TCP header\r\n");
        Log("  Source port      "); LogF("%hu",     TcpHdrSrcPort          ); Log("\r\n");
        Log("  Destination port "); LogF("%hu",     TcpHdrDstPort          ); Log("\r\n");
        Log("  Seq number       "); LogF("%u",      TcpHdrSeqNum           ); Log("\r\n");
        Log("  Ack number       "); LogF("%u",      TcpHdrAckNum           ); Log("\r\n");
        Log("  Header size      "); LogF("%u",      headerSize             ); Log("\r\n");
        Log("  Flags           " ); logFlags(                              ); Log("\r\n");
        Log("  Window           "); LogF("%hu",     TcpHdrWindow           ); Log("\r\n");
        Log("  Checksum (hex)   "); LogF("%04hX",   TcpHdrChecksum         ); Log("\r\n");
        Log("  Calculated (hex) "); LogF("%04hX",   calculatedChecksum     ); Log("\r\n");
        Log("  Urgent pointer   "); LogF("%hu",     TcpHdrUrgent           ); Log("\r\n");

    }
    else
    {
        LogF("TCP   header %hu >>> %hu", TcpHdrSrcPort, TcpHdrDstPort);
        logFlags();
        Log("\r\n");
    }
}
void TcpHdrReadFromPacket(char* pPacket)
{
    int lastRestartPoint = RestartPoint;
    RestartPoint = FAULT_POINT_TcpHdrReadFromPacket;
                        
    NetInvert16(&TcpHdrSrcPort,  hdrPtrSrcPort   (pPacket));
    NetInvert16(&TcpHdrDstPort,  hdrPtrDstPort   (pPacket));
    NetInvert32(&TcpHdrSeqNum,   hdrPtrSeqnum    (pPacket));
    NetInvert32(&TcpHdrAckNum,   hdrPtrAcknum    (pPacket));
    headerSize    =            (*hdrPtrDataOffset(pPacket) >> 2) & 0xFC; //Same as right shifting by 4 bits and multiplying by 4
    uint8_t flags =             *hdrPtrFlags     (pPacket);
        TcpHdrURG = flags & 0x20; //indicates that the Urgent pointer field is significant
        TcpHdrACK = flags & 0x10; //indicates that the Acknowledgment field is significant. All packets after the initial SYN packet sent by the client should have this flag set.
        TcpHdrPSH = flags & 0x08; //Push function. Asks to push the buffered data to the receiving application.
        TcpHdrRST = flags & 0x04; //Reset the connection
        TcpHdrSYN = flags & 0x02; //Synchronize sequence numbers. Only the first packet sent from each end should have this flag set. Some other flags and fields change meaning based on this flag, and some are only valid for when it is set, and others when it is clear.
        TcpHdrFIN = flags & 0x01; //No more data from sender
    NetInvert16(&TcpHdrWindow,   hdrPtrWindow   (pPacket));
    NetInvert16(&TcpHdrChecksum, hdrPtrChecksum (pPacket));
    NetInvert16(&TcpHdrUrgent,   hdrPtrUrgent   (pPacket));
    
    char* pOptions     = pPacket    + HEADER_LENGTH;
    int   optionLength = headerSize - HEADER_LENGTH;

    mss = 0;
    for (char* p = pOptions; p < pOptions + optionLength; p++) //If the header size is zero then optionLength will be negative and the for loop will do nothing.
    {
        switch (*p)
        {
            case 0: RestartPoint = lastRestartPoint; return; //End of options so stop
            case 1: break;  //NOP, padding - optional used to pad to 32 bit boundary
            case 2:
                p++;
                if (*p != 4) LogTimeF("MSS option width %d when expected 4\r\n", *p);
                p++;
                mss = ((uint16_t)*p) << 8;
                p++;
                mss += *p;
                RestartPoint = lastRestartPoint;
                return;     //Got what we want so stop
            default: LogTimeF("Unrecognised TCP option %d\r\n", *p);
        }
    }
    RestartPoint = lastRestartPoint;
}

void TcpHdrWriteToPacket(char* pPacket)
{
    headerSize = HEADER_LENGTH;
    if (mss)
    {
        char* pOptions = pPacket + headerSize;
        pOptions[0]  = 2;
        pOptions[1]  = 4;
        pOptions[2]  = mss >> 8;
        pOptions[3]  = mss & 0xFF;
        headerSize += 4;
    }
    
    NetInvert16(hdrPtrDstPort   (pPacket), &TcpHdrDstPort);
    NetInvert16(hdrPtrSrcPort   (pPacket), &TcpHdrSrcPort);
    NetInvert32(hdrPtrSeqnum    (pPacket), &TcpHdrSeqNum );  //This is the sequence number of the first byte of this message
    NetInvert32(hdrPtrAcknum    (pPacket), &TcpHdrAckNum );  //This is the sequence number we expect in the next message
               *hdrPtrDataOffset(pPacket) = headerSize << 2; //Same as dividing by 4 to get bytes and left shifting by 4 bits
    uint8_t flags = 0;
    if (TcpHdrURG) flags |= 0x20; //indicates that the Urgent pointer field is significant
    if (TcpHdrACK) flags |= 0x10; //indicates that the Acknowledgment field is significant. All packets after the initial SYN packet sent by the client should have this flag set.
    if (TcpHdrPSH) flags |= 0x08; //Push function. Asks to push the buffered data to the receiving application.
    if (TcpHdrRST) flags |= 0x04; //Reset the connection
    if (TcpHdrSYN) flags |= 0x02; //Synchronize sequence numbers. Only the first packet sent from each end should have this flag set. Some other flags and fields change meaning based on this flag, and some are only valid for when it is set, and others when it is clear.
    if (TcpHdrFIN) flags |= 0x01; //No more data from sender
               *hdrPtrFlags     (pPacket) = flags;
    NetInvert16(hdrPtrWindow    (pPacket), &TcpHdrWindow);
    NetInvert16(hdrPtrUrgent    (pPacket), &TcpHdrUrgent);
    TcpHdrSetChecksum( pPacket, 0);
}

void TcpHdrMakeEmpty()
{
    TcpHdrSrcPort  = 0;
    TcpHdrDstPort  = 0;
    TcpHdrSeqNum   = 0;
    TcpHdrAckNum   = 0;
    headerSize     = HEADER_LENGTH;
    TcpHdrURG      = 0;
    TcpHdrACK      = 0;
    TcpHdrPSH      = 0;
    TcpHdrRST      = 0;
    TcpHdrSYN      = 0;
    TcpHdrFIN      = 0;
    TcpHdrWindow   = 0;
    TcpHdrChecksum = 0;
    TcpHdrUrgent   = 0;
    mss            = 0;

}