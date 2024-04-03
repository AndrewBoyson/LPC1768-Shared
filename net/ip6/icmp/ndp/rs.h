#include <stdint.h>
#include <stdbool.h>

extern bool RsTrace;
extern int  RsGetWaitingSolicitation(void* pPacket, int* pSize, uint8_t* pType, uint8_t* pCode);
