#include "web/http/http.h"

//Use http://tomeko.net/online_tools/cpp_text_escape.php   to convert from text to c-multiline
//Use http://tomeko.net/online_tools/cpp_text_unescape.php to convert from c-multiline to text

static const char* script =
#include "web-1wire-class.inc"
;
const char* WebOneWireClassDate = __DATE__;
const char* WebOneWireClassTime = __TIME__;

void WebOneWireClass()
{
    HttpOk("application/javascript; charset=UTF-8", "max-age=3600", WebOneWireClassDate, WebOneWireClassTime);
    HttpAddText(script);
}
