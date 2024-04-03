#include "web/http/http.h"
#include "web/base/web-nav-base.h"
#include "web/web-add.h"
#include "net/eth/mac.h"

void WebNetHtml()
{
    HttpOk("text/html; charset=UTF-8", "no-cache", NULL, NULL);
    WebAddHeader("Net", "settings.css", "net.js");
    WebAddNav(NET_PAGE);
    WebAddH1("Net");
                     
    WebAddH2("MAC");
    HttpAddText("<code id='ajax-mac'></code>\r\n");
    
    WebAddH2("TCP connections");
    HttpAddText("<code id='ajax-tcp'></code>\r\n");
    
    WebAddH2("Request resolution");
    WebAddAjaxInput ("", 100, "ajax-nr", "name-to-resolve");
    WebAddAjaxButton("MDNS over IPV6" , "request-ipv6-mdns" );
    WebAddAjaxButton("LLMNR over IPV6", "request-ipv6-llmnr");
    WebAddAjaxButton("DNS over IPV6"  , "request-ipv6-udns" );
    HttpAddText("<br/>\r\n");
    WebAddAjaxButton("MDNS over IPV4" , "request-ipv4-mdns" );
    WebAddAjaxButton("LLMNR over IPV4", "request-ipv4-llmnr");
    WebAddAjaxButton("DNS over IPV4"  , "request-ipv4-udns" );
    
    WebAddH2("Name cache");
    HttpAddText("<code id='ajax-dns'></code>\r\n");
    HttpAddText("<div class='line'>\r\n");
    HttpAddText("<button onclick = 'Net.clearVendorsFromLocalStorage()'>Clear vendors</button>\r\n");
    HttpAddText("</div>\r\n");
    
    WebAddEnd();
                        
}
