#include "web/http/http.h"
#include "web/web-add.h"
#include "web/base/web-nav-base.h"

void WebNet4Html()
{
    HttpOk("text/html; charset=UTF-8", "no-cache", NULL, NULL);
    WebAddHeader("Net IPv4", "settings.css", "net4.js");
    WebAddNav(NET4_PAGE);
    WebAddH1("Net IPv4");
    
    WebAddH2("ARP");
    HttpAddText("<code id='ajax-arp'></code>\r\n");
    HttpAddText("<div class='line'>\r\n");
    HttpAddText("<button onclick = 'Net.clearVendorsFromLocalStorage()'>Clear vendors</button>\r\n");
    HttpAddText("</div>\r\n");
    
    WebAddH2("DHCP");
    WebAddAjaxLabelled("IP4 address",   "ajax-local-ip"    );
    WebAddAjaxLabelled("Domain",        "ajax-domain-name" );
    WebAddAjaxLabelled("Host name",     "ajax-host-name"   );
    WebAddAjaxLabelled("NTP server",    "ajax-ntp-ip"      );
    WebAddAjaxLabelled("DNS server",    "ajax-dns-ip"      );
    WebAddAjaxLabelled("DHCP server",   "ajax-dhcp-ip"     );
    WebAddAjaxLabelled("Router",        "ajax-router-ip"   );
    WebAddAjaxLabelled("Subnet mask",   "ajax-subnet-mask" );
    WebAddAjaxLabelled("Broadcast IP",  "ajax-broadcast-ip");
    WebAddAjaxLabelled("Lease time IP", "ajax-lease-time"  );
    WebAddAjaxLabelled("Renewal T1",    "ajax-renewal-t1"  );
    WebAddAjaxLabelled("Renewal T2",    "ajax-renewal-t2"  );
    WebAddAjaxLabelled("Elapsed",       "ajax-elapsed"     );
    
    WebAddEnd();
                        
}
