#include <stdint.h>
#include <stdbool.h>

extern bool Echo6Trace;

extern int  Echo6HandleRequest(void (*traceback)(void), uint8_t* pType, uint8_t* pCode, char* payloadRx, int sizeRx, char* payloadTx, int* pSizeTx);
