#include <stdio.h>

#include "http/http.h"
#include "base/web-nav-base.h"
#include "web-this/web-nav-this.h"
#include "web-this/web-site-name.h"
#include "net/eth/mac.h"
#include "net/ip4/ip4addr.h"
#include "net/ip6/ip6addr.h"

void WebAddNavItem(int highlight, const char* href, const char* title)
{
    char *p;
    HttpAddText("<li ");
    if (highlight) p = "class='this'";
    else           p = "            ";
    HttpAddText(p);
    HttpAddText("><a href='");
    HttpAddText(href);
    HttpAddText("'>");
    HttpAddText(title);
    HttpAddText("</a></li>\r\n");
}
void WebAddNav(int page)
{
    HttpAddText("<a class='tab-shortcut' href='#main-content'>Skip to content</a>\r\n");

    HttpAddText("<nav><ul>\r\n");
    WebNavThis(page);
    WebNavBase(page);
    HttpAddText("</ul></nav>\r\n");
}

void WebAddHeader(const char* title, const char* style, const char* script)
{
    HttpAddText("<!DOCTYPE html>\r\n"
                     "<html>\r\n"
                     "<head>\r\n");
    HttpAddText("   <title>");
    HttpAddText(WEB_SITE_NAME);
    if (title)
    {
        HttpAddText(" - ");
        HttpAddText(title);
    }
    HttpAddText("</title>\r\n");

    HttpAddText("   <link rel='stylesheet' href='/base.css' type='text/css'/>\r\n");
    if (style)
    {
        HttpAddText("   <link rel='stylesheet' href='/");
        HttpAddText(style);
        HttpAddText("' type='text/css'/>\r\n");
    }
    if (script)
    {
        HttpAddText("   <script src='/");
        HttpAddText(script);
        HttpAddText("' type='text/javascript'></script>\r\n");
    }
    HttpAddText("   <meta name='viewport' content='width=device-width, initial-scale=1'>\r\n"
                     "   <link rel='icon'       href='/favicon.ico' type='image/x-icon'/>\r\n"
                     "</head>\r\n"
                     "<body>\r\n");

}
void WebAddH1(const char* pageName)
{
    HttpAddText("<h1 id='main-content'>");
    HttpAddText(WEB_SITE_NAME);
    HttpAddText(" - ");
    HttpAddText(pageName);
    HttpAddText("</h1>\r\n");
}
void WebAddH2(const char* text)
{
    HttpAddText("<h2>");
    HttpAddText(text);
    HttpAddText("</h2>\r\n");
}
void WebAddEnd()
{
    HttpAddText("</body>\r\n"
                "</html>\r\n");
}

void WebAddLabelledPrefixSuffix(const char* label, const char* prefix, const char* text, const char* suffix)
{
    HttpAddText("<div class='line'>\r\n");
    HttpAddF   ("  <div>%s</div>\r\n", label);
    HttpAddF   ("  <div>%s%s%s</div>\r\n", prefix, text, suffix);
    HttpAddText("</div>\r\n");
}
void WebAddLabelledText(const char* label, const char* text)
{
    HttpAddText("<div class='line'>\r\n");
    HttpAddF   ("  <div>%s</div>\r\n", label);
    HttpAddF   ("  <div>%s</div>\r\n", text);
    HttpAddText("</div>\r\n");
}

void WebAddLabelledMac(const char* label, const char* mac)
{
    HttpAddText("<div class='line'>\r\n");
    HttpAddF   ("  <div>%s</div>\r\n", label);
    HttpAddText("  <div>"); MacHttp(mac); HttpAddText("</div>\r\n");
    HttpAddText("</div>\r\n");
}

void WebAddLabelledIp4(const char* label, uint32_t ip)
{
    HttpAddText("<div class='line'>\r\n");
    HttpAddF   ("  <div>%s</div>\r\n", label);
    HttpAddText("  <div>"); Ip4AddrHttp(ip); HttpAddText("</div>\r\n");
    HttpAddText("</div>\r\n");
}

void WebAddLabelledIp6(const char* label, const char* ip)
{
    HttpAddText("<div class='line'>\r\n");
    HttpAddF   ("  <div>%s</div>\r\n", label);
    HttpAddText("  <div>"); Ip6AddrHttp(ip); HttpAddText("</div>\r\n");
    HttpAddText("</div>\r\n");
}
void WebAddLabelledOnOff(const char* label, bool value)
{
    if (value) WebAddLabelledText(label, "On");
    else       WebAddLabelledText(label, "Off");
}
void WebAddLabelledLed(const char* label, bool value)
{
    HttpAddText("<div class='line'>\r\n");
    HttpAddF   ("  <div>%s</div>\r\n", label);
    HttpAddF   ("  <div class='led' dir='%s'></div>\r\n", value ? "rtl" : "ltr");
    HttpAddText("</div>\r\n");
}
void WebAddLabelledInt(const char* label, int value)
{
    char text[30];
    snprintf(text, sizeof(text), "%8d", value); //Right align with enough spaces so that the length is always constant. 
    WebAddLabelledText(label, text);
}
void WebAddLabelledHex(const char* label, unsigned value)
{
    char text[30];
    snprintf(text, sizeof(text), "%8x", value); //Right align with enough spaces so that the length is always constant. 
    WebAddLabelledText(label, text);
}
void WebAddInputText(const char* label, float inputwidth, const char* value, const char* action, const char* name)
{
    HttpAddF   ("<form action='%s' method='get'>\r\n", action);
    HttpAddText("<div class='line'>\r\n");
    HttpAddF   ("  <div>%s</div>\r\n", label);
    HttpAddF   ("  <input type='text' name='%s' style='width:%.1fem;' value='%s'>\r\n", name, inputwidth, value);
    HttpAddText("</div>\r\n");
    HttpAddText("<input type='submit' value='Set' style='display:none;'>\r\n");
    HttpAddF   ("</form>\r\n");

}
void WebAddInputInt(const char* label, float inputwidth, int value, const char* action, const char* name)
{    
    char text[30];
    snprintf(text, sizeof(text), "%d", value);
    WebAddInputText(label, inputwidth, text, action, name);
}
void WebAddInputButton(const char* label, const char* value, const char* action, const char* name)
{
    HttpAddF   ("<form action='%s' method='get'>\r\n", action);
    HttpAddF   ("<input type='hidden' name='%s'>\r\n", name);
    HttpAddText("<div class='line'>\r\n");
    HttpAddF   ("  <div>%s</div>\r\n", label);
    HttpAddF   ("  <input type='submit' value='%s'>\r\n", value);
    HttpAddText("</div>\r\n");
    HttpAddText("</form>\r\n");
}
void WebAddAjaxInputToggle(const char* label, const char* id, const char* name)
{
    HttpAddText("<div class='line'>\r\n");
    HttpAddF   ("  <div>%s</div>\r\n", label);
    HttpAddF   ("  <div class='toggle' id='%s' tabindex='0' dir='ltr' onclick='AjaxSendNameValue(\"%s\", \"1\")' onkeydown='return event.keyCode != 13 || AjaxSendNameValue(\"%s\", \"1\")'>\r\n", id, name, name);
    HttpAddText("    <div class='slot'></div><div class='knob'></div>\r\n");
    HttpAddText("  </div>\r\n");
    HttpAddText("</div>\r\n");
}
void WebAddAjaxLed(const char* label, const char* id)
{
    HttpAddText("<div class='line'>\r\n");
    HttpAddF   ("  <div>%s</div>\r\n", label);
    HttpAddF   ("  <div class='led' id='%s' dir='ltr'></div>\r\n", id);
    HttpAddText("</div>\r\n");
}
void WebAddAjaxButton(const char* caption, const char* name)
{
    HttpAddF   ("<button onclick='AjaxSendNameValue(\"%s\", \"1\")'>%s</button>\r\n", name, caption);
}
void WebAddAjaxInput(const char* label, float inputwidth, const char* id, const char* name)
{
    HttpAddText("<div class='line'>\r\n");
    HttpAddF   ("  <div>%s</div>\r\n", label);
    HttpAddF   ("  <input type='text' style='width:%.1fem;' id='%s' onchange='AjaxSendNameValue(\"%s\", this.value)'>\r\n", inputwidth, id, name);
    HttpAddText("</div>\r\n");
}
void WebAddAjaxInputSuffix(const char* label, float inputwidth, const char* id, const char* name, const char* suffix)
{
    HttpAddText("<div class='line'>\r\n");
    HttpAddF   ("  <div>%s</div>\r\n", label);
    HttpAddF   ("  <input type='text' style='width:%.1fem;' id='%s' onchange='AjaxSendNameValue(\"%s\", this.value)'>%s\r\n", inputwidth, id, name, suffix);
    HttpAddText("</div>\r\n");
}
void WebAddAjaxLabelled(const char* label, const char* id)
{
    HttpAddText("<div class='line'>\r\n");
    HttpAddF   ("  <div>%s</div>\r\n", label);
    HttpAddF   ("  <div id='%s'></div>\r\n", id);
    HttpAddText("</div>\r\n");
}
void WebAddAjaxLabelledSuffix(const char* label, const char* id, const char* suffix)
{
    HttpAddText("<div class='line'>\r\n");
    HttpAddF   ("  <div>%s</div>\r\n", label);
    HttpAddF   ("  <div><span id='%s'></span>%s</div>\r\n", id, suffix);
    HttpAddText("</div>\r\n");
}
void WebAddAjaxInputLabelId(const char* labelId, float inputwidth, const char* id, const char* name)
{
    HttpAddText("<div class='line'>\r\n");
    HttpAddF   ("  <div id='%s'></div>\r\n", labelId);
    HttpAddF   ("  <input type='text' style='width:%.1fem;' id='%s' onchange='AjaxSendNameValue(\"%s\", this.value)'>\r\n", inputwidth, id, name);
    HttpAddText("</div>\r\n");
}


