#include <stdbool.h>
#include <stdint.h>

#include "net/net.h"

char* Ip6HdrPtrVersionTrafficFlow (char* pPacket) { return pPacket +   0; } //4
char* Ip6HdrPtrPayloadLength      (char* pPacket) { return pPacket +   4; } //2
char* Ip6HdrPtrProtocol           (char* pPacket) { return pPacket +   6; } //1
char* Ip6HdrPtrHopLimit           (char* pPacket) { return pPacket +   7; } //1
char* Ip6HdrPtrSrc                (char* pPacket) { return pPacket +   8; } //16
char* Ip6HdrPtrDst                (char* pPacket) { return pPacket +  24; } //16
const int IP6_HEADER_LENGTH = 40;

int  Ip6HdrGetVersion   (char* pPacket) { int32_t r; NetDirect32(&r, Ip6HdrPtrVersionTrafficFlow(pPacket)); return (r >> 4) & 0x0F; }
int  Ip6HdrGetPayloadLen(char* pPacket) { int16_t r; NetInvert16(&r, Ip6HdrPtrPayloadLength     (pPacket)); return  r;              }
int  Ip6HdrGetProtocol  (char* pPacket) { return                     *Ip6HdrPtrProtocol         (pPacket);                          }
int  Ip6HdrGetHopLimit  (char* pPacket) { return                     *Ip6HdrPtrHopLimit         (pPacket);                          }

void Ip6HdrSetVersion   (char* pPacket, int value) { value <<= 4; NetDirect32(Ip6HdrPtrVersionTrafficFlow(pPacket), &value); }
void Ip6HdrSetPayloadLen(char* pPacket, int value) {              NetInvert16(Ip6HdrPtrPayloadLength     (pPacket), &value); }
void Ip6HdrSetProtocol  (char* pPacket, int value) {                         *Ip6HdrPtrProtocol          (pPacket) = value;  }
void Ip6HdrSetHopLimit  (char* pPacket, int value) {                         *Ip6HdrPtrHopLimit          (pPacket) = value;  }
