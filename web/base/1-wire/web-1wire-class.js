//OneWire class
'use strict';

class OneWire
{
    static DS18B20ToString(value)
    {
        switch (value)
        {
            case 0x7FFF: return 'CRC error'                     ;
            case 0x7FFE: return 'ROM not found'                 ;
            case 0x7FFD: return 'Timed out'                     ;
            case 0x7FFC: return 'No device detected after reset';
            case 0x7FFB: return 'Device removed during search'  ;
            case 0x7FFA: return 'Value not set'                 ;
        }
        return (value / 16.0).toFixed(1);
    }
}
