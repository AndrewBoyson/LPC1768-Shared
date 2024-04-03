#include "web/http/http.h"
#include "web/base/web-nav-base.h"
#include "web/web-add.h"
#include "log/log.h"

void WebLogHtml()
{
    HttpOk("text/html; charset=UTF-8", "no-cache", NULL, NULL);
    WebAddHeader("Log", "settings.css", NULL);
    WebAddNav(LOG_PAGE);
    WebAddH1("Log");
    
                 WebAddInputButton("Existing content",      "Clear",    "/log", "clearlog"    );
    if (LogUart) WebAddInputButton("Output to uart is on",  "Turn off", "/log", "chg-log-uart");
    else         WebAddInputButton("Output to uart is off", "Turn on",  "/log", "chg-log-uart");
    
    HttpAddText("<code>");
    HttpAddStream(LogEnumerateStart, LogEnumerate);
    HttpAddText("</code>");
    
    WebAddEnd();
}
