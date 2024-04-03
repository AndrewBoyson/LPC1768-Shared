#include <stdbool.h>

extern bool RaTrace;

extern int  RaHandleReceivedAdvertisement(void (*traceback)(void), void* pPacket, int* pSize);
