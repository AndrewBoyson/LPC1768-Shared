#include <stdint.h>
#include <stdbool.h>

#include "log/log.h"
#include "net/net.h"
#include "net/action.h"
#include "udp.h"
#include "ntp/ntp.h"
#include "ntp/ntpclient.h"
#include "tftp/tftp.h"
#include "dhcp/dhcp.h"
#include "dns/dns.h"
#include "net/eth/eth.h"
#include "net/ip4/ip4.h"
#include "net/ip6/ip6.h"
#include "net/ip6/slaac.h"
#include "net/ip6/icmp/ndp/ns.h"
#include "net/user.h"

bool UdpTrace = true;

static char* hdrPtrSrcPort    (char* pPacket) { return pPacket + 0; } //2
static char* hdrPtrDstPort    (char* pPacket) { return pPacket + 2; } //2
static char* hdrPtrTotalLength(char* pPacket) { return pPacket + 4; } //2
static char* hdrPtrChecksum   (char* pPacket) { return pPacket + 6; } //2
static const int HEADER_LENGTH = 8;

void UdpHdrSetChecksum(void* pPacket, uint16_t checksum)
{
    NetDirect16(hdrPtrChecksum(pPacket), &checksum);
}
static uint16_t     srcPort;
static uint16_t     dstPort;
static uint16_t    checksum;
static uint16_t totalLength;

static void readHeader(char* pPacket)
{
    NetInvert16(&srcPort,     hdrPtrSrcPort    (pPacket));
    NetInvert16(&dstPort,     hdrPtrDstPort    (pPacket));
    NetDirect16(&checksum,    hdrPtrChecksum   (pPacket));
    NetInvert16(&totalLength, hdrPtrTotalLength(pPacket));
}
void UdpMakeHeader(int size, char* pPacket)
{
    checksum = 0;
    NetInvert16(hdrPtrSrcPort    (pPacket), &srcPort    );
    NetInvert16(hdrPtrDstPort    (pPacket), &dstPort    );
    NetDirect16(hdrPtrChecksum   (pPacket), &checksum   );
    NetInvert16(hdrPtrTotalLength(pPacket), &size       );

}

void UdpLogHeader(uint16_t calculatedChecksum)
{
    if (NetTraceVerbose)
    {
        Log ("UDP header\r\n");
        LogF("  Source port      %hu\r\n", srcPort);
        LogF("  Destination port %hu\r\n", dstPort);
        LogF("  Total length     %hu\r\n", totalLength);
        LogF("  Checksum (hex)   %04hX\r\n", checksum);
        LogF("  Calculated       %04hX\r\n", calculatedChecksum);
    }
    else
    {
        LogF("UDP   header %hu >>> %hu\r\n", srcPort, dstPort);
    }
}

static int handlePort(void (*traceback)(void), int dataLengthRx, char* pDataRx, int* pPataLengthTx, char* pDataTx)
{
    if (UserHandleReceivedUdpPacket && (dstPort == UserUdpPort1 || dstPort == UserUdpPort2)) return UserHandleReceivedUdpPacket(dstPort, traceback, dataLengthRx, pDataRx, pPataLengthTx, pDataTx);
    
    switch (dstPort)
    {
        //Handle these
        case DHCP_CLIENT_PORT:          return DhcpHandleResponse      (traceback,                     dataLengthRx, pDataRx, pPataLengthTx, pDataTx);  //   68
        case NTP_PORT:                  return  NtpHandlePacketReceived(traceback,                     dataLengthRx, pDataRx, pPataLengthTx, pDataTx);  //  123
        case DNS_UNICAST_CLIENT_PORT:   return  DnsHandlePacketReceived(traceback, DNS_PROTOCOL_UDNS,  dataLengthRx, pDataRx, pPataLengthTx, pDataTx);  //53053
        case DNS_MDNS_PORT:             return  DnsHandlePacketReceived(traceback, DNS_PROTOCOL_MDNS,  dataLengthRx, pDataRx, pPataLengthTx, pDataTx);  // 5353
        case DNS_LLMNR_CLIENT_PORT:     return  DnsHandlePacketReceived(traceback, DNS_PROTOCOL_LLMNR, dataLengthRx, pDataRx, pPataLengthTx, pDataTx);  //53055
        case DNS_LLMNR_SERVER_PORT:     return  DnsHandlePacketReceived(traceback, DNS_PROTOCOL_LLMNR, dataLengthRx, pDataRx, pPataLengthTx, pDataTx);  // 5355
        case TFTP_CLIENT_PORT:          return TftpHandlePacketReceived(traceback,                     dataLengthRx, pDataRx, pPataLengthTx, pDataTx);  //60690
        
        //Quietly drop these
        case     0:             //Unused but can be generated accidentally. For example the WIZ lights send it when powered up.
        case DHCP_SERVER_PORT:  //67
        case TFTP_SERVER_PORT:  //69
        case   137:             //NETBIOS name service
        case   138:             //NETBIOS datagram service
        case   139:             //NETBIOS session service
        case   500:             //Key exchange - Xbox live                  
        case  1900:             //SSDP Simple Service Discovery Protocol (uPnP)
        case  3074:             //Xbox live
        case  3076:             //Call of Duty - Xbox
        case  5050:             //Don't know but have been sent by Kate and my android phones.
        case  5224:             //Don't know but a burst was broadcast by Kate's phone containing '_logitech-reverse-bonjour._tcp.local.5446 192.168.1.36 string'
        case  5684:             //CoAps
        case  9956:             //Alljoyn part of Allseen IoT services
        case  9997:             //VLC
        case  9998:             //VLC
        case  9999:             //VLC
        case 17500:             //Dropbox LAN sync
        case 38899:             //WIZ
        case 38900:             //WIZ
        case 57621:             //Spotify P2P
            return DO_NOTHING;
            
        //Report anything else
        default:
            if (UdpTrace)
            {
                LogTimeF("UDP unknown port %d\r\n", dstPort);
                traceback(); //This will already include the UDP header
            }
            return DO_NOTHING;
    }
}
int UdpHandleReceivedPacket(void (*traceback)(void), int sizeRx, char* pPacketRx, int* pSizeTx, char* pPacketTx)
{    
    readHeader(pPacketRx);

    void* pDataRx    = pPacketRx + HEADER_LENGTH;
    void* pDataTx    = pPacketTx + HEADER_LENGTH;
    int dataLengthRx =    sizeRx - HEADER_LENGTH;
    int dataLengthTx =  *pSizeTx - HEADER_LENGTH;
    
    int action = handlePort(traceback, dataLengthRx, pDataRx, &dataLengthTx, pDataTx);
    
    *pSizeTx = dataLengthTx + HEADER_LENGTH;
    
    uint16_t tmpPort = dstPort;
    dstPort = srcPort;
    srcPort = tmpPort;
    
    return action;
}
static int pollForPacketToSend(int type, int* pDataLength, char* pData)
{
    int action = DO_NOTHING;
    
    if (!action && type == ETH_IPV4) //DHCP only works under IPv4
    {
        action = DhcpPollForRequestToSend(pData, pDataLength);
        if (action)
        {
            srcPort = DHCP_CLIENT_PORT;
            dstPort = DHCP_SERVER_PORT;
        }
    }
    
    //if (!action && type == (DnsSendRequestsViaIp4 ? ETH_IPV4 : ETH_IPV6)) //DNS is agnostic
    if (!action) //DNS is agnostic
    {
        action = DnsPollForPacketToSend(type, pData, pDataLength);
        int dest = ActionGetDestPart(action);
        if (dest)
        {
            switch (dest)
            {
                case   UNICAST_DNS:    srcPort = DNS_UNICAST_CLIENT_PORT;   dstPort = DNS_UNICAST_SERVER_PORT;   break; //53053,   53
                case MULTICAST_MDNS:   srcPort = DNS_MDNS_PORT;             dstPort = DNS_MDNS_PORT;             break; // 5353, 5353
                case MULTICAST_LLMNR:  srcPort = DNS_LLMNR_CLIENT_PORT;     dstPort = DNS_LLMNR_SERVER_PORT;     break; //53055, 5355
                
                //Report anything else
                default:
                    LogTimeF("DNS unknown dest %d\r\n", dest);
                    return DO_NOTHING;
            }
        }
    }
    if (!action && type == (NtpClientQuerySendRequestsViaIp4 ? ETH_IPV4 : ETH_IPV6)) //NTP is agnostic
    {
        action = NtpPollForPacketToSend(type, pData, pDataLength);
        if (action)
        {
            srcPort = NTP_PORT;
            dstPort = NTP_PORT;
        }
    }
    if (!action && type == (TftpSendRequestsViaIp4 ? ETH_IPV4 : ETH_IPV6)) //TFTP is agnostic
    {
        action = TftpPollForPacketToSend(type, pData, pDataLength);
        if (action)
        {
            srcPort = TFTP_CLIENT_PORT;
            dstPort = TFTP_SERVER_PORT;
        }
    }
    if (UserPollForUdpPacketToSend)
    {
        if (!action)
        {
            action = UserPollForUdpPacketToSend(type, pDataLength, pData);
            if (action)
            {
                srcPort = UserUdpDstPort;
                dstPort = UserUdpSrcPort;
            }
        }
    }
    
    return action;
}
int UdpPollForPacketToSend(int type, int* pSize, char* pPacket)
{
    void* pData    = pPacket + HEADER_LENGTH;
    int dataLength =  *pSize - HEADER_LENGTH;
    
    int action = pollForPacketToSend(type, &dataLength, pData);
    
    *pSize = dataLength + HEADER_LENGTH;
    return action;
}
