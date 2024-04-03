#include <stdbool.h>
#include <stdint.h>
#include "ntp.h"

extern bool     NtpClientTrace;
extern void     NtpClientInit(void);
extern void     NtpClientTimeUpdateSuccessful(void);

extern int32_t  NtpClientReplyOffsetMs;
extern int32_t  NtpClientReplyMaxDelayMs;
extern void     NtpClientReply(void (*traceback)(void), char* pPacket);

extern uint64_t NtpClientQueryTime;

extern char     NtpClientQueryServerName[];
extern int32_t  NtpClientQueryInitialInterval;
extern int32_t  NtpClientQueryNormalInterval;
extern int32_t  NtpClientQueryRetryInterval;

extern bool     NtpClientQuerySendRequestsViaIp4;
extern uint32_t NtpClientQueryServerIp4;
extern char     NtpClientQueryServerIp6[];

extern void     NtpClientQueryStartInterval(int type);
extern int      NtpClientQueryPoll(int type, char* pPacket, int* pSize);

enum {
    NTP_QUERY_INTERVAL_INITIAL,
    NTP_QUERY_INTERVAL_NORMAL,
    NTP_QUERY_INTERVAL_RETRY
};
