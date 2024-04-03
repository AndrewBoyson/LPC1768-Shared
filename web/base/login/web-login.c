
#include "web/base/web-pages-base.h"
#include "web-login.h"
#include "web/http/http.h"
#include "lpc1768/reset/reset.h"

int WebLoginOriginalToDo = 0;

bool WebLoginCookiesContainValidSessionId(char* pCookies)
{
    if (!WebLoginSessionIdIsSet()) return false;
    
    while (pCookies)
    {
        char* pName;
        char* pValue;
        pCookies = HttpCookiesSplit(pCookies, &pName, &pValue);
        
        if (HttpSameStr(pName, WebLoginSessionNameGet())) //HttpSameStr handles NULLs correctly
        {
            if (HttpSameStr(pValue, WebLoginSessionIdGet())) return true;
        }
    }
    return false;
}
void WebLoginForceNewPassword()
{
    WebLoginSessionIdReset();
    WebLoginPasswordReset();
}
void WebLoginInit()
{
    if (!ResetWasPushButton()) WebLoginPasswordRestore();
    WebLoginSessionNameCreate();
}