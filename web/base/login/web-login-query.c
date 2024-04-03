
#include "web/http/http.h"
#include "web/base/web-pages-base.h"
#include "web-login.h"

bool WebLoginQueryPasswordOk = false;

void WebLoginQuery(char* pQuery)
{
    WebLoginQueryPasswordOk = false;
    
    while (pQuery)
    {
        char* pName;
        char* pValue;
        pQuery = HttpQuerySplit(pQuery, &pName, &pValue);
        
        int value = HttpQueryValueAsInt(pValue);
        if (HttpSameStr(pName, "todo"     )) WebLoginOriginalToDo = value;
        
        HttpQueryUnencode(pValue);
        if (HttpSameStr(pName, "password" ))
        {
            if (!WebLoginPasswordIsSet()) //This is if there has been a normal reset: not a fault nor a power on.
            {
                WebLoginPasswordSet(pValue);
                WebLoginQueryPasswordOk = true;
            }
            else
            {
                WebLoginQueryPasswordOk = WebLoginPasswordMatches(pValue);
            }
        }
    }
}
