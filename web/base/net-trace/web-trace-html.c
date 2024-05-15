#include "web/web-add.h"
#include "web/base/web-nav-base.h"
#include "web/http/http.h"

void WebTraceHtml()
{
    HttpOk("text/html; charset=UTF-8", "no-cache", NULL, NULL);
    WebAddHeader2("Net Trace", "settings.css", "ajax-class.js", "trace.js");
    WebAddNav(TRACE_PAGE);
    WebAddH1("Net Trace");
    
    WebAddH2("General");
    WebAddAjaxInput      ("Trace host"      , 5  , "ajax-trace-net-host"   , "set-trace-net-host"   );
    WebAddAjaxInputToggle("Trace stack"          , "ajax-trace-net-stack"  , "chg-trace-net-stack"  );
    WebAddAjaxInputToggle("Trace new line"       , "ajax-trace-net-newline", "chg-trace-net-newline");
    WebAddAjaxInputToggle("Trace verbose"        , "ajax-trace-net-verbose", "chg-trace-net-verbose");
    WebAddH2("Net");
    WebAddAjaxInputToggle("MAC"                  , "ajax-trace-link"       , "chg-trace-link"       );
    WebAddAjaxInputToggle("Ip4 filtered"         , "ajax-trace-ip4"        , "chg-trace-ip4"        );
    WebAddAjaxInputToggle("Ip6 filtered"         , "ajax-trace-ip6"        , "chg-trace-ip6"        );
    WebAddAjaxInputToggle("Udp filtered"         , "ajax-trace-udp"        , "chg-trace-udp"        );
    WebAddAjaxInputToggle("Tcp filtered"         , "ajax-trace-tcp"        , "chg-trace-tcp"        );
    WebAddAjaxInputToggle("Echo4 (ping4)"        , "ajax-trace-echo4"      , "chg-trace-echo4"      );
    WebAddAjaxInputToggle("Echo6 (ping6)"        , "ajax-trace-echo6"      , "chg-trace-echo6"      );
    WebAddAjaxInputToggle("Dest6 unreacheable"   , "ajax-trace-dest6"      , "chg-trace-dest6"      );
    WebAddAjaxInputToggle("HTTP"                 , "ajax-trace-http"       , "chg-trace-http"       );
    WebAddAjaxInputToggle("TFTP"                 , "ajax-trace-tftp"       , "chg-trace-tftp"       );
    WebAddH2("Send requests via IPv4");
    WebAddAjaxInputToggle("DNS request via IPv4" , "ajax-trace-dns-ip4"    , "chg-send-dns-ip4"     );
    WebAddAjaxInputToggle("NTP request via IPv4" , "ajax-trace-ntp-ip4"    , "chg-send-ntp-ip4"     );
    WebAddAjaxInputToggle("TFTP request via IPv4", "ajax-trace-tftp-ip4"   , "chg-send-tftp-ip4"    );
    WebAddH2("Router Resolution");
    WebAddAjaxInputToggle("Router advertise"     , "ajax-trace-ra"         , "chg-trace-ra"         );
    WebAddAjaxInputToggle("Router solicit"       , "ajax-trace-rs"         , "chg-trace-rs"         );
    WebAddAjaxInputToggle("DHCP"                 , "ajax-trace-dhcp"       , "chg-trace-dhcp"       );
    WebAddH2("Address Resolution");
    WebAddAjaxInputToggle("IP4 cache"            , "ajax-trace-ar4"        , "chg-trace-ar4"        );
    WebAddAjaxInputToggle("IP6 cache"            , "ajax-trace-ar6"        , "chg-trace-ar6"        );
    WebAddAjaxInputToggle("ARP"                  , "ajax-trace-arp"        , "chg-trace-arp"        );
    WebAddAjaxInputToggle("NS server"            , "ajax-trace-ns-recv-sol", "chg-trace-ns-recv-sol");
    WebAddAjaxInputToggle("NS client reply"      , "ajax-trace-ns-recv-adv", "chg-trace-ns-recv-adv");
    WebAddAjaxInputToggle("NS client query"      , "ajax-trace-ns-send-sol", "chg-trace-ns-send-sol");
    WebAddH2("Name Resolution");
    WebAddAjaxInputToggle("NR cache"             , "ajax-trace-nr"         , "chg-trace-nr"         );
    WebAddAjaxInputToggle("DNS name"             , "ajax-trace-dns-name"   , "chg-trace-dns-name"   );
    WebAddAjaxInputToggle("DNS client query"     , "ajax-trace-dns-query"  , "chg-trace-dns-query"  );
    WebAddAjaxInputToggle("DNS client reply"     , "ajax-trace-dns-reply"  , "chg-trace-dns-reply"  );
    WebAddAjaxInputToggle("DNS server"           , "ajax-trace-dns-server" , "chg-trace-dns-server" );
    WebAddH2("NTP");
    WebAddAjaxInputToggle("NTP"                  , "ajax-trace-ntp"        , "chg-trace-ntp"        );
    WebAddAjaxInputToggle("NTP client"           , "ajax-trace-ntp-client" , "chg-trace-ntp-client" );
    
    WebAddEnd();
}
