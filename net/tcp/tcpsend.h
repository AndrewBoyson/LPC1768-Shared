#include "tcb.h"

extern int  TcpSend               (int* pSize, void* pPacket, struct tcb* pTcb);
extern int  TcpResendLastAck      (int* pSize, void* pPacket, struct tcb* pTcb);
extern int  TcpResendLastUnAcked  (int* pSize, void* pPacket, struct tcb* pTcb);
extern int  TcpSendReset          (int* pSize, void* pPacket, struct tcb* pTcb);

extern int  TcpPollForPacketToSend(int* pSize, void* pPacket, int type, int* pRemArIndex, int* pLocIpScope);

