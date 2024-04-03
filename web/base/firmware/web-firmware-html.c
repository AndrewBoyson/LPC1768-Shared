#include <stdio.h>

#include "web/http/http.h"
#include "web/base/web-nav-base.h"
#include "web/web-add.h"
#include "web-firmware.h"

void WebFirmwareHtml()
{
    HttpOk("text/html; charset=UTF-8", "no-cache", NULL, NULL);
    WebAddHeader("Firmware", "settings.css", "firmware.js");
    WebAddNav(FIRMWARE_PAGE);
    WebAddH1("Firmware");
    
    WebAddH2("Existing files on the device");

    HttpAddText("<code id='list'>");
    WebFirmwareListSemihostFiles();
    HttpAddText("</code>\r\n");
    
    WebAddH2("Choose a local file");
    HttpAddText("<div><input type='file' id='fileInput'/></div>\r\n");
    
    WebAddH2("Upload the local file to the device");
    HttpAddText("<div><button onclick='startUpload();'>Upload</button></div>\r\n");
    HttpAddText("<code id='uploadresult'></code>\r\n");
    
    WebAddH2("Restart the device");
    HttpAddText("<div><button id='restartbutton' onclick='restart();'>Restart</button></div>\r\n");
    HttpAddText("<code id='restartresult'></code>\r\n");
    
    WebAddEnd();
}
