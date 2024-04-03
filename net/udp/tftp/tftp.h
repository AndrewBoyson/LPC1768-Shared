#include <stdint.h>
#include <stdbool.h>

extern bool     TftpTrace;

extern char     TftpServerName[];
extern char     TftpFileName[];
extern int      TftpWriteStatus;

#define TFTP_WRITE_STATUS_UNAVAILABLE -1
#define TFTP_WRITE_STATUS_NONE         0
#define TFTP_WRITE_STATUS_REQUEST      1
#define TFTP_WRITE_STATUS_IN_PROGRESS  2

extern bool     TftpSendRequestsViaIp4;
extern uint32_t TftpServerIp4;
extern char     TftpServerIp6[];

extern int    (*TftpGetNextByteFunction)();

extern int      TftpHandlePacketReceived(void (*traceback)(void), int sizeRx, void* pPacketRx, int* pSizeTx, void* pPacketTx);
extern int      TftpPollForPacketToSend(int type, void* pPacket, int* pSize);

#define TFTP_SERVER_PORT 69
#define TFTP_CLIENT_PORT 60690
