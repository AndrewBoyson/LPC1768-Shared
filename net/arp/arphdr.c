#include <stdint.h>

#include "net/net.h"

char* ArpHdrPtrHardwareType      (char* pPacket) { return pPacket +  0; }
char* ArpHdrPtrProtocolType      (char* pPacket) { return pPacket +  2; }
char* ArpHdrPtrHardwareLength    (char* pPacket) { return pPacket +  4; }
char* ArpHdrPtrProtocolLength    (char* pPacket) { return pPacket +  5; }
char* ArpHdrPtrOpCode            (char* pPacket) { return pPacket +  6; }
char* ArpHdrPtrSenderHardwareAddr(char* pPacket) { return pPacket +  8; }
char* ArpHdrPtrSenderProtocolAddr(char* pPacket) { return pPacket + 14; }
char* ArpHdrPtrTargetHardwareAddr(char* pPacket) { return pPacket + 18; }
char* ArpHdrPtrTargetProtocolAddr(char* pPacket) { return pPacket + 24; }
const int ARP_HEADER_LENGTH = 28;

int16_t  ArpHdrGetHardwareType      (char* pPacket) { int16_t  r; NetInvert16(&r, ArpHdrPtrHardwareType      (pPacket)); return r; }
int16_t  ArpHdrGetProtocolType      (char* pPacket) { int16_t  r; NetInvert16(&r, ArpHdrPtrProtocolType      (pPacket)); return r; }
int8_t   ArpHdrGetHardwareLength    (char* pPacket) { return         *( int8_t* )(ArpHdrPtrHardwareLength    (pPacket));           }
int8_t   ArpHdrGetProtocolLength    (char* pPacket) { return         *( int8_t* )(ArpHdrPtrProtocolLength    (pPacket));           }
int16_t  ArpHdrGetOpCode            (char* pPacket) { int16_t  r; NetInvert16(&r, ArpHdrPtrOpCode            (pPacket)); return r; }
uint32_t ArpHdrGetSenderProtocolAddr(char* pPacket) { uint32_t r; NetDirect32(&r, ArpHdrPtrSenderProtocolAddr(pPacket)); return r; }
uint32_t ArpHdrGetTargetProtocolAddr(char* pPacket) { uint32_t r; NetDirect32(&r, ArpHdrPtrTargetProtocolAddr(pPacket)); return r; }

void     ArpHdrSetHardwareType      (char* pPacket,  int16_t value) {  NetInvert16(ArpHdrPtrHardwareType      (pPacket),  &value); }
void     ArpHdrSetProtocolType      (char* pPacket,  int16_t value) {  NetInvert16(ArpHdrPtrProtocolType      (pPacket),  &value); }
void     ArpHdrSetHardwareLength    (char* pPacket,  int8_t  value) { *( int8_t* )(ArpHdrPtrHardwareLength    (pPacket)) = value ; }
void     ArpHdrSetProtocolLength    (char* pPacket,  int8_t  value) { *( int8_t* )(ArpHdrPtrProtocolLength    (pPacket)) = value ; }
void     ArpHdrSetOpCode            (char* pPacket,  int16_t value) {  NetInvert16(ArpHdrPtrOpCode            (pPacket),  &value); }
void     ArpHdrSetSenderProtocolAddr(char* pPacket, uint32_t value) {  NetDirect32(ArpHdrPtrSenderProtocolAddr(pPacket),  &value); }
void     ArpHdrSetTargetProtocolAddr(char* pPacket, uint32_t value) {  NetDirect32(ArpHdrPtrTargetProtocolAddr(pPacket),  &value); }

