#include <stdint.h>

extern char* DhcpHdrPtrOp    (char* pPacket);
extern char* DhcpHdrPtrHtype (char* pPacket);
extern char* DhcpHdrPtrHlen  (char* pPacket);
extern char* DhcpHdrPtrHops  (char* pPacket);
extern char* DhcpHdrPtrXid   (char* pPacket);
extern char* DhcpHdrPtrSecs  (char* pPacket);
extern char* DhcpHdrPtrFlags (char* pPacket);
extern char* DhcpHdrPtrCiaddr(char* pPacket);
extern char* DhcpHdrPtrYiaddr(char* pPacket);
extern char* DhcpHdrPtrSiaddr(char* pPacket);
extern char* DhcpHdrPtrGiaddr(char* pPacket);
extern char* DhcpHdrPtrChaddr(char* pPacket);
extern char* DhcpHdrPtrLegacy(char* pPacket);
extern char* DhcpHdrPtrCookie(char* pPacket);
extern const int DHCP_HEADER_LENGTH;

extern uint8_t  DhcpHdrGetOp    (char* pPacket);
extern uint8_t  DhcpHdrGetHtype (char* pPacket);
extern uint8_t  DhcpHdrGetHlen  (char* pPacket);
extern uint8_t  DhcpHdrGetHops  (char* pPacket);
extern uint32_t DhcpHdrGetXid   (char* pPacket);
extern uint16_t DhcpHdrGetSecs  (char* pPacket);
extern uint16_t DhcpHdrGetFlags (char* pPacket);
extern uint32_t DhcpHdrGetCiaddr(char* pPacket);
extern uint32_t DhcpHdrGetYiaddr(char* pPacket);
extern uint32_t DhcpHdrGetSiaddr(char* pPacket);
extern uint32_t DhcpHdrGetGiaddr(char* pPacket);
extern uint32_t DhcpHdrGetCookie(char* pPacket);

extern void DhcpHdrSetOp    (char* pPacket, uint8_t  value);
extern void DhcpHdrSetHtype (char* pPacket, uint8_t  value);
extern void DhcpHdrSetHlen  (char* pPacket, uint8_t  value);
extern void DhcpHdrSetHops  (char* pPacket, uint8_t  value);
extern void DhcpHdrSetXid   (char* pPacket, uint32_t value);
extern void DhcpHdrSetSecs  (char* pPacket, uint16_t value);
extern void DhcpHdrSetFlags (char* pPacket, uint16_t value);
extern void DhcpHdrSetCiaddr(char* pPacket, uint32_t value);
extern void DhcpHdrSetYiaddr(char* pPacket, uint32_t value);
extern void DhcpHdrSetSiaddr(char* pPacket, uint32_t value);
extern void DhcpHdrSetGiaddr(char* pPacket, uint32_t value);
extern void DhcpHdrSetCookie(char* pPacket, uint32_t value);
