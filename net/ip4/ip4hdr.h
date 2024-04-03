#include <stdbool.h>
#include <stdint.h>

extern char* Ip4HdrPtrVersionIhl (char* pPacket);
extern char* Ip4HdrPtrTos        (char* pPacket);
extern char* Ip4HdrPtrLength     (char* pPacket);
extern char* Ip4HdrPtrId         (char* pPacket);
extern char* Ip4HdrPtrFragInfo   (char* pPacket);
extern char* Ip4HdrPtrTtl        (char* pPacket);
extern char* Ip4HdrPtrProtocol   (char* pPacket);
extern char* Ip4HdrPtrChecksum   (char* pPacket);
extern char* Ip4HdrPtrSrc        (char* pPacket);
extern char* Ip4HdrPtrDst        (char* pPacket);
extern const int IP4_HEADER_LENGTH;

extern uint8_t  Ip4HdrGetVersion   (char* pPacket);
extern int      Ip4HdrGetHeaderLen (char* pPacket);
extern uint8_t  Ip4HdrGetTos       (char* pPacket);
extern uint16_t Ip4HdrGetLength    (char* pPacket);
extern uint16_t Ip4HdrGetId        (char* pPacket);
extern uint16_t Ip4HdrGetFragOffset(char* pPacket);
extern bool     Ip4HdrGetDontFrag  (char* pPacket);
extern bool     Ip4HdrGetMoreFrags (char* pPacket);
extern uint8_t  Ip4HdrGetTtl       (char* pPacket);
extern uint8_t  Ip4HdrGetProtocol  (char* pPacket);
extern uint16_t Ip4HdrGetChecksum  (char* pPacket);
extern uint32_t Ip4HdrGetSrc       (char* pPacket);
extern uint32_t Ip4HdrGetDst       (char* pPacket);

extern void Ip4HdrSetVersion    (char* pPacket, uint8_t  value);
extern void Ip4HdrSetHeaderLen  (char* pPacket, int      value);
extern void Ip4HdrSetTos        (char* pPacket, uint8_t  value);
extern void Ip4HdrSetLength     (char* pPacket, uint16_t value);
extern void Ip4HdrSetId         (char* pPacket, uint16_t value);
extern void Ip4HdrSetFragInfo   (char* pPacket, uint16_t value);
extern void Ip4HdrSetFragOffset (char* pPacket, uint16_t value);
extern void Ip4HdrSetDontFrag   (char* pPacket, bool     value);
extern void Ip4HdrSetMoreFrags  (char* pPacket, bool     value);
extern void Ip4HdrSetTtl        (char* pPacket, uint8_t  value);
extern void Ip4HdrSetProtocol   (char* pPacket, uint8_t  value);
extern void Ip4HdrSetChecksum   (char* pPacket, uint16_t value);
extern void Ip4HdrSetSrc        (char* pPacket, uint32_t value);
extern void Ip4HdrSetDst        (char* pPacket, uint32_t value);
