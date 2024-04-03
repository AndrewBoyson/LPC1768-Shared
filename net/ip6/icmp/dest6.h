#include <stdint.h>
#include <stdbool.h>

extern bool Dest6Trace;

extern int  Dest6HandleRequest(void (*traceback)(void), uint8_t* pType, uint8_t* pCode);
