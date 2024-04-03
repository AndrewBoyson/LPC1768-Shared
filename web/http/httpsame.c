#include <stdbool.h>
#include <ctype.h>
#include "http.h"

bool HttpSameStr(const char* pa, const char* pb)
{
    if (!pa || !pb) return false; //Handle NULL references
    
    while(true)
    {
        if ( *pa != *pb) return false; //If they are not the same return false
        if (!*pa)        return true;  //If finished return true;
        pa++;
        pb++;
    }
}
bool HttpSameStrCaseInsensitive(const char* pa, const char* pb)
{
    if (!pa || !pb) return false; //Handle NULL references
    
    while(true)
    {
        if ( toupper(*pa) != toupper(*pb)) return false; //If they are not the same return false
        if (!*pa)        return true;  //If finished return true;
        pa++;
        pb++;
    }
}
bool HttpSameDate(const char* date, const char* time, const char* pOtherDate)
{
    if (!pOtherDate) return false; //Not the same if no lastModified
    
    char pFileDate[HTTP_DATE_LENGTH];
    HttpDateFromDateTime(date, time, pFileDate);
    
    return HttpSameStr(pFileDate, pOtherDate);
}