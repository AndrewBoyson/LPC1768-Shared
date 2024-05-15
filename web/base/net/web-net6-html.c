#include "web/http/http.h"
#include "web/base/web-nav-base.h"
#include "web/web-add.h"

void WebNet6Html()
{
    HttpOk("text/html; charset=UTF-8", "no-cache", NULL, NULL);
    WebAddHeader3("Net IPv6", "settings.css", "ajax-class.js", "net-class.js", "net6.js");
    WebAddNav(NET6_PAGE);
    WebAddH1("Net IPv6");

    WebAddH2("NDP - Neighbour Advertisement");
    HttpAddText("<code id='ajax-arp'></code>\r\n");
    HttpAddText("<div class='line'>\r\n");
    HttpAddText("<button onclick = 'Net.clearVendorsFromLocalStorage()'>Clear vendors</button>\r\n");
    HttpAddText("</div>\r\n");
    
    WebAddH2("Link local");
    WebAddAjaxLabelled("SLAAC",                 "ajax-slaac");
    
    WebAddH2("NDP - Router Advertisement");
    WebAddAjaxLabelled("Hop limit",             "ajax-hop-limit");
    WebAddAjaxLed     ("Managed address",       "ajax-managed");
    WebAddAjaxLed     ("Other configuration",   "ajax-other");
    WebAddAjaxLabelled("Lease time",            "ajax-ndp-lease");
    WebAddAjaxLabelled("Elapsed",               "ajax-ndp-elapsed");
    
    WebAddH2("RA options");
    WebAddAjaxLabelled("Router MAC",            "ajax-router-mac");
    WebAddAjaxLabelled("MTU",                   "ajax-mtu");
    WebAddAjaxLabelled("DNS server",            "ajax-dns-ip");
    WebAddAjaxLabelled("DNS life secs",         "ajax-dns-life");
    
    WebAddH2("Unique local");
    WebAddAjaxLabelled("Prefix length",         "ajax-unique-prefix-length");
    WebAddAjaxLed     ("Prefix flag L",         "ajax-unique-prefix-l");
    WebAddAjaxLed     ("Prefix flag A",         "ajax-unique-prefix-a");
    WebAddAjaxLabelled("Prefix valid secs",     "ajax-unique-prefix-limit");
    WebAddAjaxLabelled("Prefix preferred secs", "ajax-unique-prefix-preferred");
    WebAddAjaxLabelled("Prefix",                "ajax-unique-prefix");
    
    WebAddH2("Global");
    WebAddAjaxLabelled("Prefix length",         "ajax-global-prefix-length");
    WebAddAjaxLed     ("Prefix flag L",         "ajax-global-prefix-l");
    WebAddAjaxLed     ("Prefix flag A",         "ajax-global-prefix-a");
    WebAddAjaxLabelled("Prefix valid secs",     "ajax-global-prefix-limit");
    WebAddAjaxLabelled("Prefix preferred secs", "ajax-global-prefix-preferred");
    WebAddAjaxLabelled("Prefix",                "ajax-global-prefix");
    
    WebAddEnd();
                        
}
