#include <stdint.h>
extern bool     FirmwareTrace;

extern int      FirmwareSentLength;
extern int      FirmwareRcvdLength;
extern uint32_t FirmwareSentCheckSum;
extern uint32_t FirmwareRcvdCheckSum;
extern int      FirmwareFileLength;
extern uint32_t FirmwareFileCheckSum;

extern bool     FirmwareFailed;

extern void     FirmwareStart(int contentlength);
extern bool     FirmwareAdd  (const char* pDataStart, int dataLength);

extern void     FirmwareMain(void);
