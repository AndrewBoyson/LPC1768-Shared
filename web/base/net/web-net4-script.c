#include "web/http/http.h"

//Use http://tomeko.net/online_tools/cpp_text_escape.php   to convert from text to c-multiline
//Use http://tomeko.net/online_tools/cpp_text_unescape.php to convert from c-multiline to text

const char* WebNet4ScriptDate = __DATE__;
const char* WebNet4ScriptTime = __TIME__;

static const char* script =
#include "web-net4-script.inc"
;
void WebNet4Script()
{
    HttpOk("application/javascript; charset=UTF-8", "max-age=3600", WebNet4ScriptDate, WebNet4ScriptTime);
    HttpAddText(script);
}
