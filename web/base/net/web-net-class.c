#include "web/http/http.h"

//Use http://tomeko.net/online_tools/cpp_text_escape.php   to convert from text to c-multiline
//Use http://tomeko.net/online_tools/cpp_text_unescape.php to convert from c-multiline to text

const char* WebNetClassDate = __DATE__;
const char* WebNetClassTime = __TIME__;

static const char* script =
#include "web-net-class.inc"
;
void WebNetClass()
{
    HttpOk("application/javascript; charset=UTF-8", "max-age=3600", WebNetClassDate, WebNetClassTime);
    HttpAddText(script);
}
