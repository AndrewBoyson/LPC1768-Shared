
#include "web/http/http.h"
#include "net/eth/mac.h"
#include "net/tcp/tcb.h"
#include "net/resolve/nr.h"
#include "net/resolve/nrtest.h"
#include "net/resolve/ar4.h"
#include "net/resolve/ar6.h"

void WebNetAjax()
{
    HttpOk("text/plain; charset=UTF-8", "no-cache", NULL, NULL);
    
    for (int b = 0; b < 6; b++) HttpAddByteAsHex(MacLocal[b]);
    HttpAddChar('\n');
    HttpAddText(NrTest);
    HttpAddChar('\n');
    HttpAddChar('\f');
    
    TcbSendAjax();
    HttpAddChar('\f');
    
    NrSendAjax();
    HttpAddChar('\f');
    
    Ar4SendAjax();
    HttpAddChar('\f');
    
    Ar6SendAjax();
}
