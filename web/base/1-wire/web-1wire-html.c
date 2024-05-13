#include <stdio.h>

#include "web/http/http.h"
#include "web/base/web-nav-base.h"
#include "web/web-add.h"

void WebOneWireHtml()
{
    HttpOk("text/html; charset=UTF-8", "no-cache", NULL, NULL);
    WebAddHeader("1-Wire", "settings.css", "1wire.js");
    WebAddNav(ONE_WIRE_PAGE);
    WebAddH1("1-Wire");

    WebAddH2("1-wire bus timings");
    WebAddAjaxLabelled   ("Scan time ms",        "ajax-1-wire-scan-time"    );
    WebAddAjaxLabelled   ("Low start tweak",     "ajax-1-wire-tweak-low"    );
    WebAddAjaxLabelled   ("Float start tweak",   "ajax-1-wire-tweak-float"  );
    WebAddAjaxLabelled   ("Read start tweak",    "ajax-1-wire-tweak-read"   );
    WebAddAjaxLabelled   ("High start tweak",    "ajax-1-wire-tweak-high"   );
    WebAddAjaxLabelled   ("Release start tweak", "ajax-1-wire-tweak-release");
    WebAddAjaxInputToggle("One wire trace",      "ajax-1-wire-trace"        , "onewiretrace");

    WebAddH2("DS18B20 1-wire devices");
    HttpAddText("<code id='ajax-device-values'></code>\r\n");
    
    WebAddH2("ROMs");
    WebAddAjaxInputLabelId("ajax-name-0", 11, "ajax-rom-0", "rom0");
    WebAddAjaxInputLabelId("ajax-name-1", 11, "ajax-rom-1", "rom1");
    WebAddAjaxInputLabelId("ajax-name-2", 11, "ajax-rom-2", "rom2");
    WebAddAjaxInputLabelId("ajax-name-3", 11, "ajax-rom-3", "rom3");
    WebAddAjaxInputLabelId("ajax-name-4", 11, "ajax-rom-4", "rom4");
    WebAddAjaxInputLabelId("ajax-name-5", 11, "ajax-rom-5", "rom5");
    WebAddAjaxInputLabelId("ajax-name-6", 11, "ajax-rom-6", "rom6");
    WebAddAjaxInputLabelId("ajax-name-7", 11, "ajax-rom-7", "rom7");
    
    WebAddEnd();
}
