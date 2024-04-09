"//Net class\n"
"'use strict';\n"
"\n"
"//Private variables\n"
"let netMacXhr_ = null;\n"
"let netMacToFetch_ = '';\n"
"\n"
"class Net\n"
"{\n"
"    //Public functions\n"
"    static addVendorToLocalStorage(mac)\n"
"    {\n"
"        if (netMacXhr_ && netMacXhr_.readyState != 4) return; //Do nothing if there is an ongoing request\n"
"        netMacToFetch_ = mac;\n"
"        netMacXhr_ = new XMLHttpRequest();\n"
"        netMacXhr_.onreadystatechange = function()\n"
"        {\n"
"            if (netMacXhr_.readyState == 4)\n"
"            {\n"
"                if (netMacXhr_.status == 200)\n"
"                {\n"
"                    if (netMacXhr_.responseText == '')\n"
"                    {\n"
"                        localStorage.setItem(netMacToFetch_, 'Empty vendor');\n"
"                    }\n"
"                    else\n"
"                    {\n"
"                        localStorage.setItem(netMacToFetch_, netMacXhr_.responseText);\n"
"                    }\n"
"                }\n"
"                else\n"
"                {\n"
"                    localStorage.setItem(netMacToFetch_, netMacXhr_.status);\n"
"                }\n"
"            }\n"
"        };\n"
"        netMacXhr_.open('GET', 'https://api.macaddress.io/v1?apiKey=at_dzgreB2SuPBDdy5mwTvivH3R3cvkH&search=' + netMacToFetch_, true);\n"
"        netMacXhr_.send();\n"
"    }\n"
"    static getVendorFromLocalStorage(mac)\n"
"    {\n"
"        if (mac[1] === '2' ||\n"
"            mac[1] === '6' ||\n"
"            mac[1] === 'a' ||\n"
"            mac[1] === 'A' ||\n"
"            mac[1] === 'e' ||\n"
"            mac[1] === 'E'    ) return '(Locally administered)';\n"
"            \n"
"        return localStorage.getItem(mac);\n"
"    }\n"
"    static clearVendorsFromLocalStorage()\n"
"    {\n"
"        localStorage.clear();\n"
"    }\n"
"    static makeIp4(text)\n"
"    {\n"
"        let result = '';\n"
"        result += parseInt(text.substr(6, 2), 16).toString();\n"
"        result += '.';\n"
"        result += parseInt(text.substr(4, 2), 16).toString();\n"
"        result += '.';\n"
"        result += parseInt(text.substr(2, 2), 16).toString();\n"
"        result += '.';\n"
"        result += parseInt(text.substr(0, 2), 16).toString();\n"
"        return result;\n"
"    }\n"
"    static makeIp4Rev(text)\n"
"    {\n"
"        let result = '';\n"
"        result += parseInt(text.substr(0, 2), 16).toString();\n"
"        result += '.';\n"
"        result += parseInt(text.substr(2, 2), 16).toString();\n"
"        result += '.';\n"
"        result += parseInt(text.substr(4, 2), 16).toString();\n"
"        result += '.';\n"
"        result += parseInt(text.substr(6, 2), 16).toString();\n"
"        return result;\n"
"    }\n"
"    static makeMac(text)\n"
"    {\n"
"        text = text.toLowerCase();\n"
"        let result = '';\n"
"        result += text.substr( 0, 2);\n"
"        result += ':';\n"
"        result += text.substr( 2, 2);\n"
"        result += ':';\n"
"        result += text.substr( 4, 2);\n"
"        result += ':';\n"
"        result += text.substr( 6, 2);\n"
"        result += ':';\n"
"        result += text.substr( 8, 2);\n"
"        result += ':';\n"
"        result += text.substr(10, 2);\n"
"        return result;\n"
"    }\n"
"\n"
"    static hexToBit(text, iBit)\n"
"    {\n"
"       let value = parseInt(text, 16);\n"
"       value >>= iBit;\n"
"       return value & 1;\n"
"    }\n"
"    static makeIp6(text)\n"
"    {\n"
"        function makeWord(text)\n"
"        {\n"
"            let word = parseInt(text, 16);\n"
"            if (word === 0) return '';\n"
"            return word.toString(16);\n"
"        }\n"
"        text = text.toLowerCase();\n"
"        let result = '';\n"
"        result += makeWord(text.substr( 0, 4));\n"
"        result += ':';\n"
"        result += makeWord(text.substr( 4, 4));\n"
"        result += ':';\n"
"        result += makeWord(text.substr( 8, 4));\n"
"        result += ':';\n"
"        result += makeWord(text.substr(12, 4));\n"
"        result += ':';\n"
"        result += makeWord(text.substr(16, 4));\n"
"        result += ':';\n"
"        result += makeWord(text.substr(20, 4));\n"
"        result += ':';\n"
"        result += makeWord(text.substr(24, 4));\n"
"        result += ':';\n"
"        result += makeWord(text.substr(28, 4));\n"
"        return result;\n"
"    }\n"
"}"