#include <stdbool.h>
#include <stdint.h>

extern char* Ip6HdrPtrVersionTrafficFlow (char* pPacket);
extern char* Ip6HdrPtrPayloadLength      (char* pPacket);
extern char* Ip6HdrPtrProtocol           (char* pPacket);
extern char* Ip6HdrPtrHopLimit           (char* pPacket);
extern char* Ip6HdrPtrSrc                (char* pPacket);
extern char* Ip6HdrPtrDst                (char* pPacket);
extern const int IP6_HEADER_LENGTH;

extern int  Ip6HdrGetVersion   (char* pPacket);
extern int  Ip6HdrGetPayloadLen(char* pPacket);
extern int  Ip6HdrGetProtocol  (char* pPacket);
extern int  Ip6HdrGetHopLimit  (char* pPacket);

extern void Ip6HdrSetVersion   (char* pPacket, int value);
extern void Ip6HdrSetPayloadLen(char* pPacket, int value);
extern void Ip6HdrSetProtocol  (char* pPacket, int value);
extern void Ip6HdrSetHopLimit  (char* pPacket, int value);
