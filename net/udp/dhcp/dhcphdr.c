#include <stdint.h>

#include "net/net.h"

char* DhcpHdrPtrOp    (char* pPacket) { return pPacket +   0; } //1
char* DhcpHdrPtrHtype (char* pPacket) { return pPacket +   1; } //1
char* DhcpHdrPtrHlen  (char* pPacket) { return pPacket +   2; } //1
char* DhcpHdrPtrHops  (char* pPacket) { return pPacket +   3; } //1
char* DhcpHdrPtrXid   (char* pPacket) { return pPacket +   4; } //4
char* DhcpHdrPtrSecs  (char* pPacket) { return pPacket +   8; } //2
char* DhcpHdrPtrFlags (char* pPacket) { return pPacket +  10; } //2
char* DhcpHdrPtrCiaddr(char* pPacket) { return pPacket +  12; } //4
char* DhcpHdrPtrYiaddr(char* pPacket) { return pPacket +  16; } //4
char* DhcpHdrPtrSiaddr(char* pPacket) { return pPacket +  20; } //4
char* DhcpHdrPtrGiaddr(char* pPacket) { return pPacket +  24; } //4
char* DhcpHdrPtrChaddr(char* pPacket) { return pPacket +  28; } //16
char* DhcpHdrPtrLegacy(char* pPacket) { return pPacket +  44; } //192
char* DhcpHdrPtrCookie(char* pPacket) { return pPacket + 236; } //4
const int DHCP_HEADER_LENGTH = 240;

uint8_t  DhcpHdrGetOp    (char* pPacket) { return                     *DhcpHdrPtrOp    (pPacket);            }
uint8_t  DhcpHdrGetHtype (char* pPacket) { return                     *DhcpHdrPtrHtype (pPacket);            }
uint8_t  DhcpHdrGetHlen  (char* pPacket) { return                     *DhcpHdrPtrHlen  (pPacket);            }
uint8_t  DhcpHdrGetHops  (char* pPacket) { return                     *DhcpHdrPtrHops  (pPacket);            }
uint32_t DhcpHdrGetXid   (char* pPacket) { uint32_t r; NetInvert32(&r, DhcpHdrPtrXid   (pPacket)); return r; }
uint16_t DhcpHdrGetSecs  (char* pPacket) { uint16_t r; NetInvert16(&r, DhcpHdrPtrSecs  (pPacket)); return r; }
uint16_t DhcpHdrGetFlags (char* pPacket) { uint16_t r; NetInvert16(&r, DhcpHdrPtrFlags (pPacket)); return r; }
uint32_t DhcpHdrGetCiaddr(char* pPacket) { uint32_t r; NetDirect32(&r, DhcpHdrPtrCiaddr(pPacket)); return r; }
uint32_t DhcpHdrGetYiaddr(char* pPacket) { uint32_t r; NetDirect32(&r, DhcpHdrPtrYiaddr(pPacket)); return r; }
uint32_t DhcpHdrGetSiaddr(char* pPacket) { uint32_t r; NetDirect32(&r, DhcpHdrPtrSiaddr(pPacket)); return r; }
uint32_t DhcpHdrGetGiaddr(char* pPacket) { uint32_t r; NetDirect32(&r, DhcpHdrPtrGiaddr(pPacket)); return r; }
uint32_t DhcpHdrGetCookie(char* pPacket) { uint32_t r; NetInvert32(&r, DhcpHdrPtrCookie(pPacket)); return r; }

void DhcpHdrSetOp    (char* pPacket, uint8_t  value) {            *DhcpHdrPtrOp       (pPacket) = value;  }
void DhcpHdrSetHtype (char* pPacket, uint8_t  value) {            *DhcpHdrPtrHtype    (pPacket) = value;  }
void DhcpHdrSetHlen  (char* pPacket, uint8_t  value) {            *DhcpHdrPtrHlen     (pPacket) = value;  }
void DhcpHdrSetHops  (char* pPacket, uint8_t  value) {            *DhcpHdrPtrHops     (pPacket) = value;  }
void DhcpHdrSetXid   (char* pPacket, uint32_t value) { NetInvert32(DhcpHdrPtrXid      (pPacket), &value); }
void DhcpHdrSetSecs  (char* pPacket, uint16_t value) { NetInvert16(DhcpHdrPtrSecs     (pPacket), &value); }
void DhcpHdrSetFlags (char* pPacket, uint16_t value) { NetInvert16(DhcpHdrPtrFlags    (pPacket), &value); }
void DhcpHdrSetCiaddr(char* pPacket, uint32_t value) { NetDirect32(DhcpHdrPtrCiaddr   (pPacket), &value); }
void DhcpHdrSetYiaddr(char* pPacket, uint32_t value) { NetDirect32(DhcpHdrPtrYiaddr   (pPacket), &value); }
void DhcpHdrSetSiaddr(char* pPacket, uint32_t value) { NetDirect32(DhcpHdrPtrSiaddr   (pPacket), &value); }
void DhcpHdrSetGiaddr(char* pPacket, uint32_t value) { NetDirect32(DhcpHdrPtrGiaddr   (pPacket), &value); }
void DhcpHdrSetCookie(char* pPacket, uint32_t value) { NetInvert32(DhcpHdrPtrCookie   (pPacket), &value); }
