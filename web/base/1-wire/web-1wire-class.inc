"//OneWire class\n"
"'use strict';\n"
"\n"
"class OneWire\n"
"{\n"
"    static DS18B20ToString(value)\n"
"    {\n"
"        switch (value)\n"
"        {\n"
"            case 0x7FFF: return 'CRC error'                     ;\n"
"            case 0x7FFE: return 'ROM not found'                 ;\n"
"            case 0x7FFD: return 'Timed out'                     ;\n"
"            case 0x7FFC: return 'No device detected after reset';\n"
"            case 0x7FFB: return 'Device removed during search'  ;\n"
"            case 0x7FFA: return 'Value not set'                 ;\n"
"        }\n"
"        return (value / 16.0).toFixed(1);\n"
"    }\n"
"}\n"