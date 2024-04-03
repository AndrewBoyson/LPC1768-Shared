#include <stdint.h>
#include <stdbool.h>

#include "crypto/tls/tls.h"
#include "web/http/http.h"

bool HttpsGetTrace()
{
    return HttpGetTrace() || TlsTrace;
}
void HttpsReset(int connectionId)
{
    TlsReset(connectionId);
}
bool HttpsResponse(int connectionId, bool clientFinished, int* pWindowSize, uint8_t* pWindow, uint32_t windowPositionInStream)
{
    return TlsResponse(connectionId, clientFinished, pWindowSize, pWindow, windowPositionInStream);
}
void HttpsRequest (int connectionId, int windowSize, uint8_t* pWindow, uint32_t windowPositionInStream)
{
    TlsRequest(connectionId, windowSize, pWindow, windowPositionInStream);
}