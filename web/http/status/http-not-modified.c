#include "web/http/http.h"

void HttpNotModified()
{
    HttpAddText("HTTP/1.1 304 Not Modified\r\n"
                     "Date: ");
    char pDate[HTTP_DATE_LENGTH];
    HttpDateFromNow(pDate);
    HttpAddText(pDate);
    HttpAddText("\r\n"
                     "\r\n");
}
