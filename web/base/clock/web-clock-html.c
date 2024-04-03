#include <time.h>

#include "web/http/http.h"
#include "web/base/web-nav-base.h"
#include "web/web-add.h"

void WebClockHtml()
{
    HttpOk("text/html; charset=UTF-8", "no-cache", NULL, NULL);
    WebAddHeader("Clock", "settings.css", "clock.js");
    WebAddNav(CLOCK_PAGE);
    WebAddH1("Clock");
    
    WebAddH2("Status");
    WebAddAjaxLed("RTC is set"           , "ajax-rtc-set"    );
    WebAddAjaxLed("Clock is set"         , "ajax-clock-set"  );
    WebAddAjaxLed("External source is ok", "ajax-source-ok"  );
    WebAddAjaxLed("Time synchronised"    , "ajax-time-locked");
    WebAddAjaxLed("Rate synchronised"    , "ajax-rate-locked");
    
    WebAddH2("Server UTC time");
    HttpAddText("<div id='ajax-date-utc'></div>\r\n");

    WebAddH2("Server local time");
    HttpAddText("<div id='ajax-date-pc'></div>\r\n");
    
    WebAddH2("Server - PC (ms)");
    HttpAddText("<div id='ajax-date-diff'></div>\r\n");    
    
    WebAddH2("UTC");
    WebAddAjaxInputToggle("Enable epoch change"     ,    "ajax-leap-enable"  , "chg-clock-leap-enable" );
    WebAddAjaxInputToggle("Direction of next epoch" ,    "ajax-leap-forward" , "chg-clock-leap-forward");
    WebAddAjaxInput      ("Year next epoch starts"  , 4, "ajax-leap-year"    , "set-clock-leap-year"   );
    WebAddAjaxInput      ("Month next epoch starts" , 4, "ajax-leap-month"   , "set-clock-leap-month"  );
    WebAddAjaxInput      ("Current era offset"      , 4, "ajax-leap-count"   , "set-clock-leap-count"  );

    HttpAddText("<div><button type='button' onclick='displayLeap()'>Display leap</button></div>\r\n");
    
    HttpAddText("<div>The leap seconds list is available <a href='https://data.iana.org/time-zones/data/leap-seconds.list' target='_blank'>here</a></div>\r\n");
        
    WebAddH2("Governor");
    WebAddAjaxInput      ("Ppb"                     , 5, "ajax-ppb"          , "ppb"           );
    WebAddAjaxInput      ("Ppb divisor"             , 5, "ajax-ppb-divisor"  , "ppbdivisor"    );
    WebAddAjaxInput      ("Ppb max change"          , 5, "ajax-ppb-max-chg"  , "ppbmaxchange"  );
    WebAddAjaxInput      ("Ppb synced limit"        , 5, "ajax-ppb-syn-lim"  , "syncedlimitppb");
    WebAddAjaxInput      ("Ppb synced hysteresis"   , 5, "ajax-ppb-syn-hys"  , "syncedhysppb"  );
    WebAddAjaxInput      ("Offset divisor"          , 5, "ajax-off-divisor"  , "slewdivisor"   );
    WebAddAjaxInput      ("Offset max (ms)"         , 5, "ajax-off-max"      , "slewmax"       );
    WebAddAjaxInput      ("Offset synced limit (ms)", 5, "ajax-off-syn-lim"  , "syncedlimitns" );
    WebAddAjaxInput      ("Offset synced hys (ms)"  , 5, "ajax-off-syn-hys"  , "syncedhysns"   );
    WebAddAjaxInput      ("Offset reset limit (s)"  , 5, "ajax-off-rst-lim"  , "maxoffsetsecs" );
    WebAddAjaxInputToggle("Trace"                      , "ajax-gov-trace"    , "clockgovtrace" );

    WebAddH2("NTP");
    WebAddAjaxInput      ("Server url"              , 5, "ajax-ntp-server"   , "ntpserver"     );
    WebAddAjaxInput      ("Initial interval (s)"    , 5, "ajax-ntp-initial"  , "clockinitial"  );
    WebAddAjaxInput      ("Normal interval (m)"     , 5, "ajax-ntp-normal"   , "clocknormal"   );
    WebAddAjaxInput      ("Retry interval (s)"      , 5, "ajax-ntp-retry"    , "clockretry"    );
    WebAddAjaxInput      ("Offset (ms)"             , 5, "ajax-ntp-offset"   , "clockoffset"   );
    WebAddAjaxInput      ("Max delay (ms)"          , 5, "ajax-ntp-max-delay", "clockmaxdelay" );

    WebAddH2("Scan times");
    WebAddAjaxLabelled   ("Program cycles avg", "ajax-scan-avg");
    WebAddAjaxLabelled   ("Program cycles max", "ajax-scan-max");
    WebAddAjaxLabelled   ("Program cycles min", "ajax-scan-min");

    WebAddEnd();
}
