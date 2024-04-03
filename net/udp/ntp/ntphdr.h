
extern char* NtpHdrPtrRefIdentifier(char* pPacket);
extern const int NTP_HEADER_LENGTH;

extern uint8_t  NtpHdrGetMode        (char* pPacket);
extern uint8_t  NtpHdrGetVersion     (char* pPacket);
extern uint8_t  NtpHdrGetLI          (char* pPacket);
extern uint8_t  NtpHdrGetStratum     (char* pPacket);
extern  int8_t  NtpHdrGetPoll        (char* pPacket);
extern  int8_t  NtpHdrGetPrecision   (char* pPacket);
extern uint32_t NtpHdrGetRootDelay   (char* pPacket);
extern uint32_t NtpHdrGetDispersion  (char* pPacket);
extern uint64_t NtpHdrGetRefTimeStamp(char* pPacket);
extern uint64_t NtpHdrGetOriTimeStamp(char* pPacket);
extern uint64_t NtpHdrGetRecTimeStamp(char* pPacket);
extern uint64_t NtpHdrGetTraTimeStamp(char* pPacket);

extern void     NtpHdrSetMode        (char* pPacket,  uint8_t value);
extern void     NtpHdrSetVersion     (char* pPacket,  uint8_t value);
extern void     NtpHdrSetLI          (char* pPacket,  uint8_t value);
extern void     NtpHdrSetStratum     (char* pPacket,  uint8_t value);
extern void     NtpHdrSetPoll        (char* pPacket,   int8_t value);
extern void     NtpHdrSetPrecision   (char* pPacket,   int8_t value);
extern void     NtpHdrSetRootDelay   (char* pPacket, uint32_t value);
extern void     NtpHdrSetDispersion  (char* pPacket, uint32_t value);
extern void     NtpHdrSetRefTimeStamp(char* pPacket, uint64_t value);
extern void     NtpHdrSetOriTimeStamp(char* pPacket, uint64_t value);
extern void     NtpHdrSetRecTimeStamp(char* pPacket, uint64_t value);
extern void     NtpHdrSetTraTimeStamp(char* pPacket, uint64_t value);