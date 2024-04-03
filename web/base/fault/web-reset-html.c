#include <stdio.h>

#include "web/http/http.h"
#include "web/base/web-nav-base.h"
#include "web/web-add.h"
#include "lpc1768/reset/reset.h"
#include "lpc1768/reset/restart.h"
#include "lpc1768/reset/rsid.h"

void WebResetHtml()
{
    HttpOk("text/html; charset=UTF-8", "no-cache", NULL, NULL);
    WebAddHeader("Last reset", "settings.css", NULL);
    WebAddNav(RESET_PAGE);
    WebAddH1("Last reset");
    
    WebAddH2("Reset source id (RSID)");
    
    WebAddLabelledLed("Power on"                       , RsidPor );
    WebAddLabelledLed("Brown out"                      , RsidBodr);
    WebAddLabelledLed("Watchdog"                       , RsidWdtr);
    WebAddLabelledLed("Restart (external or semi-host)", RsidExtr);

    WebAddH2("Restart from GUI, PB or fault");
    WebAddLabelledText("Cause", RestartGetCauseString());
    WebAddLabelledText("Zone" , RestartGetZoneString() );
    WebAddLabelledInt ("Point", RestartGetLastPoint()  );
    
    WebAddH2("Test");
    WebAddInputButton("Create hard fault",  "Test",  "/reset", "resettest");
    
    WebAddH2("Alarm");
    WebAddLabelledLed("Alarm", ResetHasAlarm());
    WebAddInputButton("Accept alarm", "Accept", "/reset", "resetaccept");
    
    WebAddEnd();
}

