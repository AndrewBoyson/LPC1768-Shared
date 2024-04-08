#include "web/http/http.h"
#include "lpc1768/reset/restart.h"
#include "web/base/web-pages-base.h"

void WebFirmwareQuery(char* pQuery)
{
    while (pQuery)
    {
        char* pName;
        char* pValue;
        pQuery = HttpQuerySplit(pQuery, &pName, &pValue);
		
        //int value = HttpQueryValueAsInt(pValue);    
        
        if (HttpSameStr(pName, "restart")) Restart(RESTART_CAUSE_RELOAD_PROGRAM, 0);
    }
}
