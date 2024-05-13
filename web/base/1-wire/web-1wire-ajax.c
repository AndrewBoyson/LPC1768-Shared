#include  <stdint.h>
#include   <stdio.h>

#include "web/http/http.h"
#include "1-wire/ds18b20.h"
#include "1-wire/1-wire.h"
#include "1-wire/1-wire-bus.h"
#include "1-wire/1-wire-device.h"

static void addRomToHttp(char* pRom)
{
    for (char* p = pRom; p < pRom + 8; p++) HttpAddByteAsHex(*p);
}

void WebOneWireAjax()
{
    HttpOk("text/plain; charset=UTF-8", "no-cache", NULL, NULL);
    
    HttpAddInt16AsHex (DeviceScanMs);           HttpAddChar('\n');
    HttpAddInt16AsHex (OneWireBusLowTweak);     HttpAddChar('\n');
    HttpAddInt16AsHex (OneWireBusFloatTweak);   HttpAddChar('\n');
    HttpAddInt16AsHex (OneWireBusReadTweak);    HttpAddChar('\n');
    HttpAddInt16AsHex (OneWireBusHighTweak);    HttpAddChar('\n');
    HttpAddInt16AsHex (OneWireBusReleaseTweak); HttpAddChar('\n');
    HttpAddNibbleAsHex(OneWireTrace);           HttpAddChar('\n');
    HttpAddChar('\f');
    
    for (int device = 0; device < DeviceCount; device++)
    {
        addRomToHttp     (  DeviceList   + device * 8);
        HttpAddInt16AsHex(*(DS18B20Value + device)   );
        HttpAddChar('\n');
    }
    HttpAddChar('\f');
    
    for (int rom = 0; rom < DS18B20RomCount; rom++)
    {
        addRomToHttp(DS18B20Roms + rom * 8);
        HttpAddText(DS18B20RomNames[rom]);
        HttpAddChar('\n');
    }
}

