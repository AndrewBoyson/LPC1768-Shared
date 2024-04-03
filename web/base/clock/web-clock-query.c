#include "web/http/http.h"
#include "clock/clk/clkgov.h"
#include "clock/clk/clkutc.h"
#include "settings/settings.h"

void WebClockQuery(char* pQuery)
{
    while (pQuery)
    {
        char* pName;
        char* pValue;
        pQuery = HttpQuerySplit(pQuery, &pName, &pValue);
        int value = HttpQueryValueAsInt(pValue);
        
        if (HttpSameStr(pName, "chg-clock-leap-enable" )) ClkUtcTglNextLeapEnable ();
        if (HttpSameStr(pName, "chg-clock-leap-forward")) ClkUtcTglNextLeapForward();
        
        int months1970 = ClkUtcGetNextEpochMonth1970();
        int months     = months1970 % 12;
        int years      = months1970 / 12;
        
        if (HttpSameStr(pName, "set-clock-leap-year"    ))
        {
            years = value - 1970;
            if (years < 0) years = 0;
            ClkUtcSetNextEpochMonth1970(years * 12 + months);     
        }
        if (HttpSameStr(pName, "set-clock-leap-month"   ))
        {
            months = value - 1;
            if (months < 0) months = 0;
            ClkUtcSetNextEpochMonth1970(years * 12 + months);     
        }
        if (HttpSameStr(pName, "set-clock-leap-count"   ))
        {
            uint16_t leaps = value;
            ClkUtcSetEpochOffsetWithoutUtcChange(leaps);     
        }
        
        if (HttpSameStr(pName, "ppb"           )) ClkGovSetPpb               (value           );
        if (HttpSameStr(pName, "slewdivisor"   )) SetClockSlewDivisor        (value           );
        if (HttpSameStr(pName, "slewmax"       )) SetClockSlewMaxMs          (value           );
        if (HttpSameStr(pName, "ppbdivisor"    )) SetClockPpbDivisor         (value           );
        if (HttpSameStr(pName, "ppbmaxchange"  )) SetClockPpbChangeMax       (value           );
        if (HttpSameStr(pName, "syncedlimitns" )) SetClockSyncedLimitNs      (value * 1000000 );
        if (HttpSameStr(pName, "syncedhysns"   )) SetClockSyncedHysterisNs   (value * 1000000 );
        if (HttpSameStr(pName, "syncedlimitppb")) SetClockSyncedLimitPpb     (value           );
        if (HttpSameStr(pName, "syncedhysppb"  )) SetClockSyncedHysterisPpb  (value           );
        if (HttpSameStr(pName, "maxoffsetsecs" )) SetClockMaxOffsetSecs      (value           );
        if (HttpSameStr(pName, "clockgovtrace" )) ChgTraceSync();
        
        if (HttpSameStr(pName, "ntpserver"     )) SetNtpClientServerName     (pValue          );
        if (HttpSameStr(pName, "clockinitial"  )) SetNtpClientInitialInterval(value           );
        if (HttpSameStr(pName, "clocknormal"   )) SetNtpClientNormalInterval (value *      60 );
        if (HttpSameStr(pName, "clockretry"    )) SetNtpClientRetryInterval  (value           );
        if (HttpSameStr(pName, "clockoffset"   )) SetNtpClientOffsetMs       (value           );
        if (HttpSameStr(pName, "clockmaxdelay" )) SetNtpClientMaxDelayMs     (value           );

    }
}

