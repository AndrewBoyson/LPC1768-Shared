#include <stdbool.h>

#include "web/http/http.h"
#include "lpc1768/reset/reset.h"

void WebResetQuery(char* pQuery)
{
    while (pQuery)
    {
        char* pName;
        char* pValue;
        pQuery = HttpQuerySplit(pQuery, &pName, &pValue);
        
        if (HttpSameStr(pName, "resetaccept")) ResetAccepted = true;
        
        if (HttpSameStr(pName, "hardfaulttest" ))
        {
            *(volatile int *)0 = 0; //Dereferencing address 0 will hard fault the processor
        }
        if (HttpSameStr(pName, "watchdogtest" ))
        {
            while(1) {;} //An endless loop will trip the watchdog
        }
        
    }
}
