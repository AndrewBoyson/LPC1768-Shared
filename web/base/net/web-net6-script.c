#include "web/http/http.h"

//Use http://tomeko.net/online_tools/cpp_text_escape.php   to convert from text to c-multiline
//Use http://tomeko.net/online_tools/cpp_text_unescape.php to convert from c-multiline to text

const char* WebNet6ScriptDate = __DATE__;
const char* WebNet6ScriptTime = __TIME__;

static const char* script =
#include "web/common/web-ajax-class.inc"
#include "web-net-class.inc"
#include "web-net6-script.inc"
;
void WebNet6Script()
{
    HttpOk("application/javascript; charset=UTF-8", "max-age=3600", WebNet6ScriptDate, WebNet6ScriptTime);
    HttpAddText(script);
}
