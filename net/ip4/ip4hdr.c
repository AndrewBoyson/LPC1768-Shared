#include <stdbool.h>
#include <stdint.h>

#include "net/net.h"

char* Ip4HdrPtrVersionIhl (char* pPacket) { return pPacket +   0; } //1
char* Ip4HdrPtrTos        (char* pPacket) { return pPacket +   1; } //1
char* Ip4HdrPtrLength     (char* pPacket) { return pPacket +   2; } //2
char* Ip4HdrPtrId         (char* pPacket) { return pPacket +   4; } //2
char* Ip4HdrPtrFragInfo   (char* pPacket) { return pPacket +   6; } //2
char* Ip4HdrPtrTtl        (char* pPacket) { return pPacket +   8; } //1
char* Ip4HdrPtrProtocol   (char* pPacket) { return pPacket +   9; } //1
char* Ip4HdrPtrChecksum   (char* pPacket) { return pPacket +  10; } //2
char* Ip4HdrPtrSrc        (char* pPacket) { return pPacket +  12; } //4
char* Ip4HdrPtrDst        (char* pPacket) { return pPacket +  16; } //4
const int IP4_HEADER_LENGTH = 20;

uint8_t  Ip4HdrGetVersion    (char* pPacket) { return                     *Ip4HdrPtrVersionIhl (pPacket) >> 4;                }
int      Ip4HdrGetHeaderLen  (char* pPacket) { return                    (*Ip4HdrPtrVersionIhl (pPacket) & 0xF) << 2;         }
uint8_t  Ip4HdrGetTos        (char* pPacket) { return                     *Ip4HdrPtrTos        (pPacket);                     }
uint16_t Ip4HdrGetLength     (char* pPacket) { uint16_t r; NetInvert16(&r, Ip4HdrPtrLength     (pPacket)); return r;          }
uint16_t Ip4HdrGetId         (char* pPacket) { uint16_t r; NetInvert16(&r, Ip4HdrPtrId         (pPacket)); return r;          }
uint16_t Ip4HdrGetFragOffset (char* pPacket) { uint16_t r; NetInvert16(&r, Ip4HdrPtrFragInfo   (pPacket)); return r & 0x1FFF; }
bool     Ip4HdrGetDontFrag   (char* pPacket) { uint16_t r; NetInvert16(&r, Ip4HdrPtrFragInfo   (pPacket)); return r & 0x4000; }
bool     Ip4HdrGetMoreFrags  (char* pPacket) { uint16_t r; NetInvert16(&r, Ip4HdrPtrFragInfo   (pPacket)); return r & 0x8000; }
uint8_t  Ip4HdrGetTtl        (char* pPacket) { return                     *Ip4HdrPtrTtl        (pPacket);                     }
uint8_t  Ip4HdrGetProtocol   (char* pPacket) { return                     *Ip4HdrPtrProtocol   (pPacket);                     }
uint16_t Ip4HdrGetChecksum   (char* pPacket) { uint16_t r; NetDirect16(&r, Ip4HdrPtrChecksum   (pPacket)); return r;          }
uint32_t Ip4HdrGetSrc        (char* pPacket) { uint32_t r; NetDirect32(&r, Ip4HdrPtrSrc        (pPacket)); return r;          }
uint32_t Ip4HdrGetDst        (char* pPacket) { uint32_t r; NetDirect32(&r, Ip4HdrPtrDst        (pPacket)); return r;          }

void Ip4HdrSetVersion    (char* pPacket, uint8_t  value) { char*   p = Ip4HdrPtrVersionIhl (pPacket); *p = (*p & 0x0F) | (value << 4); }
void Ip4HdrSetHeaderLen  (char* pPacket, int      value) { char*   p = Ip4HdrPtrVersionIhl (pPacket); *p = (*p & 0xF0) | (value >> 2); }
void Ip4HdrSetTos        (char* pPacket, uint8_t  value) {            *Ip4HdrPtrTos        (pPacket) = value;  }
void Ip4HdrSetLength     (char* pPacket, uint16_t value) { NetInvert16(Ip4HdrPtrLength     (pPacket), &value); }
void Ip4HdrSetId         (char* pPacket, uint16_t value) { NetInvert16(Ip4HdrPtrId         (pPacket), &value); }
void Ip4HdrSetFragInfo   (char* pPacket, uint16_t value) { NetInvert16(Ip4HdrPtrFragInfo   (pPacket), &value); }
void Ip4HdrSetFragOffset     (char* pPacket, uint16_t value)
{
    uint16_t flagsOffset;
    NetInvert16(&flagsOffset, Ip4HdrPtrFragInfo(pPacket));
    flagsOffset &= 0xC000;
    flagsOffset |= value;
    NetInvert16(Ip4HdrPtrFragInfo(pPacket), &flagsOffset);
}
void Ip4HdrSetDontFrag   (char* pPacket, bool     value)
{
    uint16_t flagsOffset;
    NetInvert16(&flagsOffset, Ip4HdrPtrFragInfo(pPacket));
    flagsOffset &= 0xBFFF;
    if (value)  flagsOffset |= 0x4000;
    NetInvert16(Ip4HdrPtrFragInfo(pPacket), &flagsOffset);
}
void Ip4HdrSetMoreFrags  (char* pPacket, bool     value)
{
    uint16_t flagsOffset;
    NetInvert16(&flagsOffset, Ip4HdrPtrFragInfo(pPacket));
    flagsOffset &= 0x7FFF;
    if (value)  flagsOffset |= 0x8000;
    NetInvert16(Ip4HdrPtrFragInfo(pPacket), &flagsOffset);
}
void Ip4HdrSetTtl        (char* pPacket, uint8_t  value) {            *Ip4HdrPtrTtl        (pPacket) = value;  }
void Ip4HdrSetProtocol   (char* pPacket, uint8_t  value) {            *Ip4HdrPtrProtocol   (pPacket) = value;  }
void Ip4HdrSetChecksum   (char* pPacket, uint16_t value) { NetDirect16(Ip4HdrPtrChecksum   (pPacket), &value); } //Don't invert the checksum
void Ip4HdrSetSrc        (char* pPacket, uint32_t value) { NetDirect32(Ip4HdrPtrSrc        (pPacket), &value); }
void Ip4HdrSetDst        (char* pPacket, uint32_t value) { NetDirect32(Ip4HdrPtrDst        (pPacket), &value); }
