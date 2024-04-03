#include "web/http/http.h"

static const char* cssBase =
#include "web-base-css.inc"
;
const char* WebBaseCssDate = __DATE__;
const char* WebBaseCssTime = __TIME__;

void WebBaseCss()
{
    HttpOk("text/css; charset=UTF-8", "max-age=3600", WebBaseCssDate, WebBaseCssTime);
    HttpAddText(cssBase);
}
