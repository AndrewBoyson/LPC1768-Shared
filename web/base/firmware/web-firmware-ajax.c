#include   <stdio.h>

#include "web/http/http.h"
#include "lpc1768/firmware.h"
#include "web-firmware.h"

void WebFirmwareAjax()
{
    //Header
    HttpOk("text/plain; charset=UTF-8", "no-cache", NULL, NULL);
    
    //Upload status
    if (FirmwareSentLength != FirmwareFileLength)
    {
        HttpAddText("Length error\r\n");
        HttpAddF   ("  sent %6d\r\n", FirmwareSentLength);
        HttpAddF   ("  rcvd %6d\r\n", FirmwareRcvdLength);
        HttpAddF   ("  file %6d\r\n", FirmwareFileLength);
    }
    
    //Save status
    if (FirmwareFailed)
    {
        HttpAddText("Save failed - see log");
    }
    else
    {
        HttpAddF("Saved %d bytes", FirmwareFileLength);
    }
    
    //Delimiter
    HttpAddChar('\f');
    
    //New directory list
    WebFirmwareListSemihostFiles();
}
