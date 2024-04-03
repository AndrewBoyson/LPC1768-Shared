#ifndef TCB_H
#define TCB_H

#include <stdint.h>
#include <stdbool.h>

#define TCB_EMPTY          0
#define TCB_SYN_RECEIVED   1
#define TCB_ESTABLISHED    2
#define TCB_CLOSE_FIN_WAIT 3

struct tcb
{
    int      state;
    uint32_t timeSendsBeingAcked; //Used for RTO
    int      countSendsNotAcked;  //Used for RTO
    uint32_t timeLastRcvd;        //Used for detect idle links
    int      ipType;
    int      remArIndex;          //Unique per remote ip when taken with the ipType
    int      locIpScope;          //Unique per local ip
    uint16_t remPort;
    uint16_t locPort;
    uint32_t remMss;
    uint32_t window;
    
    uint32_t remIsn;
    uint32_t bytesRcvdFromRem;
    uint32_t bytesAckdToRem;
    bool     rcvdFin;
    
    uint32_t locIsn;
    uint32_t bytesSentToRem;
    uint32_t bytesAckdByRem;
    bool     sentFin;
};

extern uint32_t    TcbGetIsn     (void);
extern struct tcb* TcbGetExisting(int ipType, int remArIndex, int locIpScope, uint16_t remPort, uint16_t locPort);
extern struct tcb* TcbGetEmpty   (void);
extern struct tcb* TcbGetNext    (struct tcb* pTcb);
extern int         TcbGetId      (struct tcb* pTcb); //A unique identifier greater than 0. An id of zero means empty.
extern void        TcbInit       (void);
extern void        TcbSendAjax   (void);

#endif