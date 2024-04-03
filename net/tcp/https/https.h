#include <stdint.h>
#include <stdbool.h>

extern bool HttpsGetTrace(void);
extern void HttpsReset   (int connectionId);
extern bool HttpsResponse(int connectionId, bool clientFinished, int* pWindowSize, uint8_t* pWindow, uint32_t windowPositionInStream);
extern void HttpsRequest (int connectionId,                      int   windowSize, uint8_t* pWindow, uint32_t windowPositionInStream);