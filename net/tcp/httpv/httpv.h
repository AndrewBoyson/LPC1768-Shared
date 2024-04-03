#include <stdint.h>
#include <stdbool.h>

extern bool HttpvGetTrace(void);
extern void HttpvReset   (int connectionId);
extern bool HttpvResponse(int connectionId, bool clientFinished, int* pWindowSize, uint8_t* pWindow, uint32_t windowPositionInStream);
extern void HttpvRequest (int connectionId,                      int   windowSize, uint8_t* pWindow, uint32_t windowPositionInStream);
