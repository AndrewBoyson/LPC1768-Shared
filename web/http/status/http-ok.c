#include "log/log.h"
#include "web/http/http.h"

char* HttpOkCookieName;
char* HttpOkCookieValue;
int   HttpOkCookieMaxAge = -1; //-1 = none, 0 = clear, +ve values = number of seconds

void HttpOk(const char* contentType, const char* cacheControl, const char* lastModifiedDate, const char* lastModifiedTime)
{
    char pDate[HTTP_DATE_LENGTH];
    
    if (!contentType)  LogTimeF("HtmlOk - missing Content-Type info\r\n");
    if (!cacheControl) LogTimeF("HtmlOk - missing Cache-Control info\r\n");
    
    HttpAddF("HTTP/1.1 200 OK\r\n"
             "Connection: close\r\n"
             "Content-Type: %s\r\n", contentType);
                
    HttpAddF("Cache-Control: %s\r\n", cacheControl);
    
    HttpDateFromNow(pDate);
    HttpAddF("Date: %s\r\n", pDate);
    
    if (lastModifiedDate)
    {
        HttpDateFromDateTime(lastModifiedDate, lastModifiedTime, pDate);
        HttpAddF("Last-Modified: %s\r\n", pDate);
    }
    
    if (HttpOkCookieName && HttpOkCookieValue)
    {
        HttpAddF("Set-Cookie: %s=%s", HttpOkCookieName, HttpOkCookieValue );
        if (HttpOkCookieMaxAge >= 0) HttpAddF("; Max-Age=%d",   HttpOkCookieMaxAge);
        HttpAddText("\r\n");
    }
    
    HttpAddText("\r\n");
}
