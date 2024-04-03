#include "web/http/http.h"

static const char* cssNav =
#include "web-nav-css.inc"
;
const char* WebNavCssDate = __DATE__;
const char* WebNavCssTime = __TIME__;

void WebNavCss()
{
    HttpOk("text/css; charset=UTF-8", "max-age=3600", WebNavCssDate, WebNavCssTime);
    HttpAddText(cssNav);
}
