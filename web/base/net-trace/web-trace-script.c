#include "web/http/http.h"

//Use http://tomeko.net/online_tools/cpp_text_escape.php   to convert from text to c-multiline
//Use http://tomeko.net/online_tools/cpp_text_unescape.php to convert from c-multiline to text

const char* WebTraceScriptDate = __DATE__;
const char* WebTraceScriptTime = __TIME__;

static const char* script =
#include "web/common/web-ajax-class.inc"
#include "web-trace-script.inc"
;
void WebTraceScript()
{
    HttpOk("application/javascript; charset=UTF-8", "max-age=3600", WebTraceScriptDate, WebTraceScriptTime);
    HttpAddText(script);
}
