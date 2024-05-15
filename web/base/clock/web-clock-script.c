#include "web/http/http.h"

//Use http://tomeko.net/online_tools/cpp_text_escape.php   to convert from text to c-multiline
//Use http://tomeko.net/online_tools/cpp_text_unescape.php to convert from c-multiline to text

const char* WebClockScriptDate = __DATE__;
const char* WebClockScriptTime = __TIME__;

static const char* script =
#include "web-clock-script.inc"
;
void WebClockScript()
{
    HttpOk("application/javascript; charset=UTF-8", "max-age=3600", WebClockScriptDate, WebClockScriptTime);
    HttpAddText(script);
}
