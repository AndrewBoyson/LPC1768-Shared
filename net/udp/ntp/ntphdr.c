#include <stdint.h>

#include "net/net.h"

static char*    hdrPtrFlags        (char* pPacket) { return pPacket +  0; } //1
static char*    hdrPtrStratum      (char* pPacket) { return pPacket +  1; } //1
static char*    hdrPtrPoll         (char* pPacket) { return pPacket +  2; } //1
static char*    hdrPtrPrecision    (char* pPacket) { return pPacket +  3; } //1
static char*    hdrPtrRootDelay    (char* pPacket) { return pPacket +  4; } //4
static char*    hdrPtrDispersion   (char* pPacket) { return pPacket +  8; } //4
       char* NtpHdrPtrRefIdentifier(char* pPacket) { return pPacket + 12; } //4
static char*    hdrPtrRefTimeStamp (char* pPacket) { return pPacket + 16; } //8
static char*    hdrPtrOriTimeStamp (char* pPacket) { return pPacket + 24; } //8
static char*    hdrPtrRecTimeStamp (char* pPacket) { return pPacket + 32; } //8
static char*    hdrPtrTraTimeStamp (char* pPacket) { return pPacket + 40; } //8
const int NTP_HEADER_LENGTH = 48;


uint8_t  NtpHdrGetLI          (char* pPacket) { return                     *hdrPtrFlags       (pPacket) >> 6       ; }
uint8_t  NtpHdrGetVersion     (char* pPacket) { return                    (*hdrPtrFlags       (pPacket) >> 3) & 0x7; }
uint8_t  NtpHdrGetMode        (char* pPacket) { return                     *hdrPtrFlags       (pPacket)       & 0x7; }
uint8_t  NtpHdrGetStratum     (char* pPacket) { return                     *hdrPtrStratum     (pPacket);             }
 int8_t  NtpHdrGetPoll        (char* pPacket) { return                     *hdrPtrPoll        (pPacket);             }
 int8_t  NtpHdrGetPrecision   (char* pPacket) { return                     *hdrPtrPrecision   (pPacket);             }
uint32_t NtpHdrGetRootDelay   (char* pPacket) { uint32_t r; NetInvert32(&r, hdrPtrRootDelay   (pPacket)); return r;  }
uint32_t NtpHdrGetDispersion  (char* pPacket) { uint32_t r; NetInvert32(&r, hdrPtrDispersion  (pPacket)); return r;  }
uint64_t NtpHdrGetRefTimeStamp(char* pPacket) { uint64_t r; NetInvert64(&r, hdrPtrRefTimeStamp(pPacket)); return r;  }
uint64_t NtpHdrGetOriTimeStamp(char* pPacket) { uint64_t r; NetInvert64(&r, hdrPtrOriTimeStamp(pPacket)); return r;  }
uint64_t NtpHdrGetRecTimeStamp(char* pPacket) { uint64_t r; NetInvert64(&r, hdrPtrRecTimeStamp(pPacket)); return r;  }
uint64_t NtpHdrGetTraTimeStamp(char* pPacket) { uint64_t r; NetInvert64(&r, hdrPtrTraTimeStamp(pPacket)); return r;  }

void NtpHdrSetLI          (char* pPacket, uint8_t  value) { char*   p = hdrPtrFlags       (pPacket); *p &= 0x3F; *p |= (value << 6); }
void NtpHdrSetVersion     (char* pPacket, uint8_t  value) { char*   p = hdrPtrFlags       (pPacket); *p &= 0xC7; *p |= (value << 3); }
void NtpHdrSetMode        (char* pPacket, uint8_t  value) { char*   p = hdrPtrFlags       (pPacket); *p &= 0xF8; *p |=  value;       }
void NtpHdrSetStratum     (char* pPacket, uint8_t  value) {            *hdrPtrStratum     (pPacket) = value;                         }
void NtpHdrSetPoll        (char* pPacket,  int8_t  value) {            *hdrPtrPoll        (pPacket) = value;                         }
void NtpHdrSetPrecision   (char* pPacket,  int8_t  value) {            *hdrPtrPrecision   (pPacket) = value;                         }
void NtpHdrSetRootDelay   (char* pPacket, uint32_t value) { NetInvert32(hdrPtrRootDelay   (pPacket), &value);                        }
void NtpHdrSetDispersion  (char* pPacket, uint32_t value) { NetInvert32(hdrPtrDispersion  (pPacket), &value);                        }
void NtpHdrSetRefTimeStamp(char* pPacket, uint64_t value) { NetInvert64(hdrPtrRefTimeStamp(pPacket), &value);                        }
void NtpHdrSetOriTimeStamp(char* pPacket, uint64_t value) { NetInvert64(hdrPtrOriTimeStamp(pPacket), &value);                        }
void NtpHdrSetRecTimeStamp(char* pPacket, uint64_t value) { NetInvert64(hdrPtrRecTimeStamp(pPacket), &value);                        }
void NtpHdrSetTraTimeStamp(char* pPacket, uint64_t value) { NetInvert64(hdrPtrTraTimeStamp(pPacket), &value);                        }