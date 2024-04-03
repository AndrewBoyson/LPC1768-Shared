#include "web/http/http.h"

//Use http://tomeko.net/online_tools/cpp_text_escape.php   to convert from text to c-multiline
//Use http://tomeko.net/online_tools/cpp_text_unescape.php to convert from c-multiline to text

const char* WebFirmwareScriptDate = __DATE__;
const char* WebFirmwareScriptTime = __TIME__;

static const char* script =
#include "web-firmware-script.inc"
;
void WebFirmwareScript()
{
    HttpOk("application/javascript; charset=UTF-8", "max-age=3600", WebFirmwareScriptDate, WebFirmwareScriptTime);
    HttpAddText(script);
}
