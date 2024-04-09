#include "ntp.h"

extern bool  NtpServerEnable;
extern int   NtpServerRequest(void (*traceback)(void), char* pPacketRx, char* pPacketTx);