#pragma once

extern bool       NtpTrace;

extern int  NtpHandlePacketReceived(void (*traceback)(void), int sizeRx, char* pPacketRx, int* pSizeTx, char* pPacketTx);
extern int  NtpPollForPacketToSend(int type, char* pPacket, int* pSize);
extern void NtpLogHeader(char* pPacket);
extern void NtpInit(char ntpEnable);

#define NTP_PORT 123

#define NTP_CLIENT 3
#define NTP_SERVER 4