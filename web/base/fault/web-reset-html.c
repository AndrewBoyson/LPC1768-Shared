#include <stdio.h>
#include <time.h>

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
    
    WebAddLabelledLed("Power on"                          , RsidPor );
    WebAddLabelledLed("Brown out"                         , RsidBodr);
    WebAddLabelledLed("Watchdog (not used directly)"      , RsidWdtr);
    WebAddLabelledLed("External (pushbutton or semi-host)", RsidExtr);
    WebAddLabelledLed("System reset"                      , RsidSysReset);
    WebAddLabelledLed("Lockup"                            , RsidLockup);

    WebAddH2("Last restart");
    WebAddLabelledText("Cause"                , RestartGetCauseString());
    WebAddLabelledHex ("Program Counter (Hex)", RestartGetLastPC     ());
    
    WebAddH2("Test");
    WebAddInputButton("Create hard     fault",  "Test",  "/reset", "hardfaulttest");
    WebAddInputButton("Create watchdog fault",  "Test",  "/reset", "watchdogtest");
    
    WebAddH2("Alarm");
    WebAddLabelledLed("Alarm", ResetHasAlarm());
    WebAddInputButton("Accept alarm", "Accept", "/reset", "resetaccept");
    
    WebAddEnd();
}

