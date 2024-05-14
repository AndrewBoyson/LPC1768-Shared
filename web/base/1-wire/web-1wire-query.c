#include <string.h>
#include <stdlib.h>
#include "web/http/http.h"
#include "1-wire/1-wire.h"
#include "1-wire/1-wire-device.h"
#include "1-wire/ds18b20.h"
#include "settings/settings.h"

void WebOneWireQuery(char* pQuery)
{
    while (pQuery)
    {
        char* pName;
        char* pValue;
        pQuery = HttpQuerySplit(pQuery, &pName, &pValue);
        
        HttpQueryUnencode(pValue);

        //int value = HttpQueryValueAsInt(pValue);
                    
        if (HttpSameStr(pName, "rom0")) { char rom[8]; DeviceParseAddress(pValue, rom); DS18B20RomSetters[0](rom); }
        if (HttpSameStr(pName, "rom1")) { char rom[8]; DeviceParseAddress(pValue, rom); DS18B20RomSetters[1](rom); }
        if (HttpSameStr(pName, "rom2")) { char rom[8]; DeviceParseAddress(pValue, rom); DS18B20RomSetters[2](rom); }
        if (HttpSameStr(pName, "rom3")) { char rom[8]; DeviceParseAddress(pValue, rom); DS18B20RomSetters[3](rom); }
        if (HttpSameStr(pName, "rom4")) { char rom[8]; DeviceParseAddress(pValue, rom); DS18B20RomSetters[4](rom); }
        if (HttpSameStr(pName, "rom5")) { char rom[8]; DeviceParseAddress(pValue, rom); DS18B20RomSetters[5](rom); }
        if (HttpSameStr(pName, "rom6")) { char rom[8]; DeviceParseAddress(pValue, rom); DS18B20RomSetters[6](rom); }
        if (HttpSameStr(pName, "rom7")) { char rom[8]; DeviceParseAddress(pValue, rom); DS18B20RomSetters[7](rom); }
        
        if (HttpSameStr(pName, "onewiretrace"  )) { OneWireTrace = !OneWireTrace; }
    }
}
