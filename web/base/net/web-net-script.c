#include "web/http/http.h"

//Use http://tomeko.net/online_tools/cpp_text_escape.php   to convert from text to c-multiline
//Use http://tomeko.net/online_tools/cpp_text_unescape.php to convert from c-multiline to text

const char* WebNetScriptDate = __DATE__;
const char* WebNetScriptTime = __TIME__;

static const char* script =
#include "web/common/web-ajax-class.inc"
#include "web-net-class.inc"
#include "web-net-script.inc"
;
void WebNetScript()
{
    HttpOk("application/javascript; charset=UTF-8", "max-age=3600", WebNetScriptDate, WebNetScriptTime);
    HttpAddText(script);
}
