#include "web/web.h"
#include "web/http/http.h"
#include "web/http/http-connection.h"
#include "web/base/web-server-base.h"
#include "web-this/web-server-this.h"
#include "web/base/web-pages-base.h"
#include "log/log.h"
#include "lpc1768/mstimer/mstimer.h"

#define LOGIN_DELAY_MS 200

#define DO_LOGIN DO_SERVER + 0

bool WebTrace = false;

int WebDecideWhatToDo(char *pPath, char* pLastModified)
{
    if (HttpSameStr(pPath, "/login")) return DO_LOGIN;
    
    int todo;
    todo = WebServerBaseDecideWhatToDo(pPath, pLastModified); if (todo != DO_NOT_FOUND) return todo;
    todo = WebServerThisDecideWhatToDo(pPath, pLastModified); if (todo != DO_NOT_FOUND) return todo;
    return DO_NOT_FOUND;
}
int WebHandleQuery(char* pQuery, char* pCookies, int* pTodo, uint32_t* pDelayUntil) //return -1 on stop; 0 on continue
{
    //If what to do is NOTHING, NOT_FOUND or NOT_MODIFIED then no query or post will be valid so stop now
    if (*pTodo < DO_LOGIN) return -1;
    
    //If what to do is LOGIN then the user has just returned the login form
    if (*pTodo == DO_LOGIN)
    {
        WebLoginQuery(pQuery);                    //Read the password and the original location
        if (WebLoginQueryPasswordOk)
        {
            if (!WebLoginSessionIdIsSet())           //If there isn't a session id already
            {
                WebLoginSessionIdNew();              //Create a new session id
            }
            *pTodo =  WebLoginOriginalToDo;          //Load the original todo and SEND_SESSION_ID
            *pTodo += DO_SEND_SESSION_ID;
        }
        *pDelayUntil = MsTimerCount + LOGIN_DELAY_MS; //To prevent brute forcing the hash delay the reply to the login
        return -1;                                    //Either way no query or post will be valid
    }
    
    //Have a normal request so authenticate
    if (!WebLoginCookiesContainValidSessionId(pCookies))
    {
        WebLoginOriginalToDo = *pTodo; //Record the original destination for redirection
        *pTodo = DO_LOGIN;
        return -1; //Ignore any query or post as the user is not authenticated
    }

    if (WebServerBaseHandleQuery(*pTodo, pQuery)) return 0;
    if (WebServerThisHandleQuery(*pTodo, pQuery)) return 0;
    return 0;
}
void WebHandlePost(int todo, int contentLength, int contentStart, int size, char* pRequestStream, uint32_t positionInRequestStream, bool* pComplete)
{
    if (WebServerBasePost(todo, contentLength, contentStart, size, pRequestStream, positionInRequestStream, pComplete)) return;
    if (WebServerThisPost(todo, contentLength, contentStart, size, pRequestStream, positionInRequestStream, pComplete)) return;
    *pComplete = true;
}

void WebAddResponse(int todo)
{
    //Check if todo includes the need to send a cookie
    if (todo >= DO_SEND_SESSION_ID)
    {
        HttpOkCookieName   = WebLoginSessionNameGet();
        HttpOkCookieValue  = WebLoginSessionIdGet();
        HttpOkCookieMaxAge = WebLoginSessionNameLife();
        todo -= DO_SEND_SESSION_ID;
    }
    else
    {
        HttpOkCookieName   = NULL;
        HttpOkCookieValue  = NULL;
        HttpOkCookieMaxAge = -1;
    }
    
    //Try all the base modules
    switch (todo)
    {
        case DO_LOGIN:           WebLoginHtml     (); return;
        case DO_NOT_FOUND:       HttpNotFound     (); return;
        case DO_NOT_MODIFIED:    HttpNotModified  (); return;
    }
    
    //If not called then call the derived (child) module
    if (WebServerBaseReply(todo)) return;
    if (WebServerThisReply(todo)) return;
}

void WebInit()
{   
    WebLoginInit();  
}