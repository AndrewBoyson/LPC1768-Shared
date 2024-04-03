#include "http.h"
#include "http-connection.h"
#include "web/web.h"
#include "lpc1768/mstimer/mstimer.h"
#include "log/log.h"

bool HttpGetTrace()
{
    return WebTrace;
}

void HttpReset(int connection)
{
    HttpConnectionReset(connection);
}
bool HttpAdd(int connectionId, int* pWindowSize, char* pWindow, uint32_t windowPositionInStream)
{
    HttpAddStart(windowPositionInStream, *pWindowSize, pWindow); //We have something to send so start the buffer
    *pWindowSize = 0;
    
    struct HttpConnection* pConnection = HttpConnectionOrNull(connectionId);
    if (!pConnection) return false; //Protects the gap between the connection being established and the request being started
    
    int todo = pConnection->toDo; //Make a copy so that we don't modify todo in the state
    
    WebAddResponse(todo);
    
    *pWindowSize = HttpAddLength();
    
    return !HttpAddFilled(); //If we haven't used a full buffer then we have finished
}
int HttpPoll(int connectionId, bool clientFinished)
{
    struct HttpConnection* pConnection = HttpConnectionOrNull(connectionId);
    if (!pConnection) //Protects the gap between the connection being established and the request being started
    {
        if (clientFinished) return HTTP_FINISHED; //The client hasn't requested anything and never will so finish
        else                return HTTP_WAIT;     //The client hasn't requested anything yet but still could
    }
    
    if (!pConnection->toDo)
    {
        if (clientFinished) return HTTP_FINISHED; //The client hasn't requested anything and never will so finish
        else                return HTTP_WAIT;     //The client hasn't requested anything yet but still could
    }
    if (!pConnection->postComplete               ) return HTTP_WAIT;  //Wait for the request (usually a POST) to finish
    if (!MsTimerAbsolute(pConnection->delayUntil)) return HTTP_WAIT;  //Wait a while (usually after a LOGIN attempt)
    
    return HTTP_HAVE_SOMETHING_TO_SEND;
}
void HttpRequest(int connectionId, int windowSize, char* pWindow, uint32_t windowPositionInStream)
{
    struct HttpConnection* pConnection;
    if (!windowPositionInStream)
    {
        pConnection = HttpConnectionNew(connectionId);
    }
    else
    {
        pConnection = HttpConnectionOrNull(connectionId);
        if (!pConnection)
        {
            LogTimeF("WebRequest - no connection corresponds to id %d\r\n", connectionId);
            return;
        }
    }
    
    pConnection->delayUntil = MsTimerCount; //Default to no delay unless modified;
    
    //Handle request for the first packet of data received but leave todo the same after that.
    int contentLength = 0;
    int contentStart  = 0;
    if (windowSize && windowPositionInStream == 0)
    {
        //Read the headers
        char* pMethod;
        char* pPath;
        char* pQuery;
        char* pLastModified;
        char* pCookies;
        contentStart = HttpRequestRead(pWindow, windowSize, &pMethod, &pPath, &pQuery, &pLastModified, &pCookies, &contentLength);
        
        //Ask the web server what to do
        pConnection->toDo = WebDecideWhatToDo(pPath, pLastModified);
        
        //Handle the query
        int stop = WebHandleQuery(pQuery, pCookies, &pConnection->toDo, &pConnection->delayUntil); //return -1 on stop; 0 on continue
        if (stop)
        {
            pConnection->postComplete = true;
            return;
        }
    }
    
    //Do the upload of anything that needs it. Todos it doesn't understand are ignored.
    if (!pConnection->postComplete) WebHandlePost(pConnection->toDo, contentLength, contentStart, windowSize, pWindow, windowPositionInStream, &pConnection->postComplete);
}
