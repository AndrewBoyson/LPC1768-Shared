
#include <stdint.h>
#include <stdbool.h>

#include "log/log.h"
#include "net/action.h"
#include "net/net.h"
#include "net/eth/eth.h"
#include "net/eth/mac.h"
#include "net/udp/dhcp/dhcp.h"
#include "net/resolve/ar4.h"
#include "net/resolve/nr.h"
#include "net/ip4/ip4addr.h"
#include "arphdr.h"

#define REQUEST   1
#define REPLY     2

bool ArpTrace = false;

uint32_t ArpAddressToResolve;
bool     ArpResolveRequestFlag = false;

static void logHeader(char* pPacket)
{    
    if (NetTraceVerbose)
    {
        LogTime("ARP header\r\n");
        if (ArpHdrGetHardwareType(pPacket) == ETHERNET) Log ("  hardwareType          = ETHERNET\r\n");
        else                                         LogF("  hardwareType          = %d\r\n", ArpHdrGetHardwareType(pPacket));
        Log ("  protocolType          = ");      EthProtocolLog(ArpHdrGetProtocolType  (pPacket));        Log("\r\n");
        LogF("  hardwareLength        = %d\r\n",                ArpHdrGetHardwareLength(pPacket));
        LogF("  protocolLength        = %d\r\n",                ArpHdrGetProtocolLength(pPacket));
        if      (ArpHdrGetOpCode(pPacket) == REQUEST)   Log ("  opCode                = REQUEST\r\n");
        else if (ArpHdrGetOpCode(pPacket) == REPLY  )   Log ("  opCode                = REPLY\r\n");
        else                                         LogF("  opCode                = %d\r\n", ArpHdrGetOpCode(pPacket));
        Log("  senderHardwareAddress = ");          MacLog(ArpHdrPtrSenderHardwareAddr(pPacket)); Log("\r\n");
        Log("  senderProtocolAddress = ");      Ip4AddrLog(ArpHdrGetSenderProtocolAddr(pPacket)); Log("\r\n");
        Log("  targetHardwareAddress = ");          MacLog(ArpHdrPtrTargetHardwareAddr(pPacket)); Log("\r\n");
        Log("  targetProtocolAddress = ");      Ip4AddrLog(ArpHdrGetTargetProtocolAddr(pPacket)); Log("\r\n");
    }
    else
    {
        Log("ARP header ");
            MacLog(ArpHdrPtrSenderHardwareAddr(pPacket)); Log("==");
        Ip4AddrLog(ArpHdrGetSenderProtocolAddr(pPacket)); Log(" >>> ");
            MacLog(ArpHdrPtrTargetHardwareAddr(pPacket)); Log("==");
        Ip4AddrLog(ArpHdrGetTargetProtocolAddr(pPacket)); Log("\r\n");
        
    }
}
static char* pHeaderTrace;
static void (*pTraceBack)(void);
static void trace()
{
    pTraceBack();
    logHeader(pHeaderTrace);
}

int ArpHandleReceivedPacket(void (*traceback)(void), char* pPacketRx, int sizeRx, char* pPacketTx, int* pSizeTx)
{
    pTraceBack = traceback;
    pHeaderTrace = pPacketRx;
    
    int16_t           hardwareType = ArpHdrGetHardwareType      (pPacketRx);
    int16_t           protocolType = ArpHdrGetProtocolType      (pPacketRx);
    int8_t          hardwareLength = ArpHdrGetHardwareLength    (pPacketRx);
    int8_t          protocolLength = ArpHdrGetProtocolLength    (pPacketRx);
    int16_t                 opCode = ArpHdrGetOpCode            (pPacketRx);
    uint32_t targetProtocolAddress = ArpHdrGetTargetProtocolAddr(pPacketRx);

    if (hardwareType          != ETHERNET     ) return DO_NOTHING; //This is not ethernet
    if (protocolType          != ETH_IPV4     ) return DO_NOTHING; //This is not IPv4
    if (hardwareLength        != 6            ) return DO_NOTHING; //This is not a MAC hardware address
    if (protocolLength        != 4            ) return DO_NOTHING; //This is not an IPv4 IP address
    if (targetProtocolAddress != DhcpLocalIp  ) return DO_NOTHING; //This packet was not addressed to us
    
    switch (opCode)
    {
        case REQUEST:
            if (ArpTrace)
            {
                if (NetTraceNewLine) Log("\r\n");
                LogTime("ARP received request\r\n");
                if (NetTraceStack) traceback();
                logHeader(pPacketRx);
            }   
                    ArpHdrSetHardwareType      (pPacketTx,  ETHERNET                              );
                    ArpHdrSetProtocolType      (pPacketTx,  ETH_IPV4                              );
                    ArpHdrSetHardwareLength    (pPacketTx,  6                                     );
                    ArpHdrSetProtocolLength    (pPacketTx,  4                                     );
                    ArpHdrSetOpCode            (pPacketTx,  REPLY                                 );
            MacCopy(ArpHdrPtrTargetHardwareAddr(pPacketTx), ArpHdrPtrSenderHardwareAddr(pPacketRx));
                    ArpHdrSetTargetProtocolAddr(pPacketTx,  ArpHdrGetSenderProtocolAddr(pPacketRx));
            MacCopy(ArpHdrPtrSenderHardwareAddr(pPacketTx), MacLocal                              );
                    ArpHdrSetSenderProtocolAddr(pPacketTx,  DhcpLocalIp                           );
            *pSizeTx = ARP_HEADER_LENGTH;
            if (ArpTrace) logHeader(pPacketTx);
            return ActionMakeFromDestAndTrace(UNICAST, ArpTrace && NetTraceStack);
            
        case REPLY:
            if (ArpTrace)
            {
                if (NetTraceNewLine) Log("\r\n");
                LogTime("ARP received reply\r\n");
                if (NetTraceStack) traceback();
                logHeader(pPacketRx);
            }   
            Ar4AddIpRecord(trace, ArpHdrPtrSenderHardwareAddr(pPacketRx), ArpHdrGetSenderProtocolAddr(pPacketRx));
            NrMakeRequestForNameFromAddress4(ArpHdrGetSenderProtocolAddr(pPacketRx));
            return DO_NOTHING;
            
        default:
            return DO_NOTHING;
    }
}
int ArpPollForPacketToSend(char* pPacketTx, int* pSizeTx)
{
    if (!ArpResolveRequestFlag) return DO_NOTHING;
    ArpResolveRequestFlag = false;

             ArpHdrSetHardwareType      (pPacketTx,  ETHERNET           );
             ArpHdrSetProtocolType      (pPacketTx,  ETH_IPV4           );
             ArpHdrSetHardwareLength    (pPacketTx,  6                  );
             ArpHdrSetProtocolLength    (pPacketTx,  4                  );
             ArpHdrSetOpCode            (pPacketTx,  REQUEST            );
    MacClear(ArpHdrPtrTargetHardwareAddr(pPacketTx)                     );
             ArpHdrSetTargetProtocolAddr(pPacketTx,  ArpAddressToResolve);
    MacCopy (ArpHdrPtrSenderHardwareAddr(pPacketTx), MacLocal           );
             ArpHdrSetSenderProtocolAddr(pPacketTx,  DhcpLocalIp        );
    *pSizeTx = ARP_HEADER_LENGTH;
    
    if (ArpTrace)
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTime("ARP send request\r\n");
        logHeader(pPacketTx);
    }
    return ActionMakeFromDestAndTrace(BROADCAST, ArpTrace && NetTraceStack);
}
