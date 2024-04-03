#include <stdbool.h>
#include <web/http/http.h>

bool HttpvGetTrace()
{
    return HttpGetTrace();
}
void HttpvReset(int connectionId)
{
    HttpReset(connectionId);
}
bool HttpvResponse(int connection, bool clientFinished, int* pWindowSize, uint8_t* pWindow, uint32_t windowPositionInStream)
{
    int status = HttpPoll(connection, clientFinished);
    bool finished = true;
    switch (status)
    {
        case HTTP_WAIT:                   finished = false;             *pWindowSize = 0;                                      break;
        case HTTP_FINISHED:               finished = true;              *pWindowSize = 0;                                      break;
        case HTTP_HAVE_SOMETHING_TO_SEND: finished = HttpAdd(connection, pWindowSize, (char*)pWindow, windowPositionInStream); break;
    }
    return finished;
}
void HttpvRequest (int connectionId, int windowSize, uint8_t* pWindow, uint32_t windowPositionInStream)
{
    HttpRequest(connectionId, windowSize, (char*)pWindow, windowPositionInStream);
}
