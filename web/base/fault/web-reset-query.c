#include <stdbool.h>

#include "web/http/http.h"
#include "lpc1768/reset/reset.h"
#include "lpc1768/reset/restart.h"

void WebResetQuery(char* pQuery)
{
    while (pQuery)
    {
        char* pName;
        char* pValue;
        pQuery = HttpQuerySplit(pQuery, &pName, &pValue);
        
        if (HttpSameStr(pName, "resetaccept")) ResetAccepted = true;
        
        if (HttpSameStr(pName, "resettest" ))
        {
            RestartPoint = 999;
            *(volatile int *)0 = 0; //Dereferencing address 0 will hard fault the processor
        }
        
    }
}
