#include "ntp.h"

extern bool  NtpServerEnable;
extern char* NtpServerName;
extern int   NtpServerRequest(void (*traceback)(void), char* pPacketRx, char* pPacketTx);