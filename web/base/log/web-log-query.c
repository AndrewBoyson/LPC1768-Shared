#include "web/http/http.h"
#include "log/log.h"
#include "settings/settings.h"

void WebLogQuery(char* pQuery)
{
    while (pQuery)
    {
        char* pName;
        char* pValue;
        pQuery = HttpQuerySplit(pQuery, &pName, &pValue);                    
        if (HttpSameStr(pName, "clearlog"    )) LogClear();
        if (HttpSameStr(pName, "chg-log-uart")) ChgLogUart();
    }
}
