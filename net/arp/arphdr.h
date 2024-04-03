#include <stdint.h>

extern char* ArpHdrPtrHardwareType      (char* pPacket);
extern char* ArpHdrPtrProtocolType      (char* pPacket);
extern char* ArpHdrPtrHardwareLength    (char* pPacket);
extern char* ArpHdrPtrProtocolLength    (char* pPacket);
extern char* ArpHdrPtrOpCode            (char* pPacket);
extern char* ArpHdrPtrSenderHardwareAddr(char* pPacket);
extern char* ArpHdrPtrSenderProtocolAddr(char* pPacket);
extern char* ArpHdrPtrTargetHardwareAddr(char* pPacket);
extern char* ArpHdrPtrTargetProtocolAddr(char* pPacket);
extern const int ARP_HEADER_LENGTH;

extern int16_t  ArpHdrGetHardwareType      (char* pPacket);
extern int16_t  ArpHdrGetProtocolType      (char* pPacket);
extern int8_t   ArpHdrGetHardwareLength    (char* pPacket);
extern int8_t   ArpHdrGetProtocolLength    (char* pPacket);
extern int16_t  ArpHdrGetOpCode            (char* pPacket);
extern uint32_t ArpHdrGetSenderProtocolAddr(char* pPacket);
extern uint32_t ArpHdrGetTargetProtocolAddr(char* pPacket);

extern void     ArpHdrSetHardwareType      (char* pPacket,  int16_t value);
extern void     ArpHdrSetProtocolType      (char* pPacket,  int16_t value);
extern void     ArpHdrSetHardwareLength    (char* pPacket,  int8_t  value);
extern void     ArpHdrSetProtocolLength    (char* pPacket,  int8_t  value);
extern void     ArpHdrSetOpCode            (char* pPacket,  int16_t value);
extern void     ArpHdrSetSenderProtocolAddr(char* pPacket, uint32_t value);
extern void     ArpHdrSetTargetProtocolAddr(char* pPacket, uint32_t value);

