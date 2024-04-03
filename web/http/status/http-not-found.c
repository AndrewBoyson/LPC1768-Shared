#include "web/http/http.h"

void HttpNotFound()
{
    HttpAddText("HTTP/1.1 404 Not Found\r\n"
                    "\r\n");
}
