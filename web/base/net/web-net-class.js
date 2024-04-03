//Net class
'use strict';

//Private variables
let netMacXhr_ = null;
let netMacToFetch_ = '';

class Net
{
    //Public functions
    static addVendorToLocalStorage(mac)
    {
        if (netMacXhr_ && netMacXhr_.readyState != 4) return; //Do nothing if there is an ongoing request
        netMacToFetch_ = mac;
        netMacXhr_ = new XMLHttpRequest();
        netMacXhr_.onreadystatechange = function()
        {
            if (netMacXhr_.readyState == 4)
            {
                if (netMacXhr_.status == 200)
                {
                    if (netMacXhr_.responseText == '')
                    {
                        localStorage.setItem(netMacToFetch_, 'Empty vendor');
                    }
                    else
                    {
                        localStorage.setItem(netMacToFetch_, netMacXhr_.responseText);
                    }
                }
                else
                {
                    localStorage.setItem(netMacToFetch_, netMacXhr_.status);
                }
            }
        };
        netMacXhr_.open('GET', 'https://api.macaddress.io/v1?apiKey=at_dzgreB2SuPBDdy5mwTvivH3R3cvkH&search=' + netMacToFetch_, true);
        netMacXhr_.send();
    }
    static getVendorFromLocalStorage(mac)
    {
        if (mac[1] === '2' ||
            mac[1] === '6' ||
            mac[1] === 'a' ||
            mac[1] === 'A' ||
            mac[1] === 'e' ||
            mac[1] === 'E'    ) return '(Locally administered)';
            
        return localStorage.getItem(mac);
    }
    static clearVendorsFromLocalStorage()
    {
        localStorage.clear();
    }
    static makeIp4(text)
    {
        let result = '';
        result += parseInt(text.substr(6, 2), 16).toString();
        result += '.';
        result += parseInt(text.substr(4, 2), 16).toString();
        result += '.';
        result += parseInt(text.substr(2, 2), 16).toString();
        result += '.';
        result += parseInt(text.substr(0, 2), 16).toString();
        return result;
    }
    static makeIp4Rev(text)
    {
        let result = '';
        result += parseInt(text.substr(0, 2), 16).toString();
        result += '.';
        result += parseInt(text.substr(2, 2), 16).toString();
        result += '.';
        result += parseInt(text.substr(4, 2), 16).toString();
        result += '.';
        result += parseInt(text.substr(6, 2), 16).toString();
        return result;
    }
    static makeMac(text)
    {
        text = text.toLowerCase();
        let result = '';
        result += text.substr( 0, 2);
        result += ':';
        result += text.substr( 2, 2);
        result += ':';
        result += text.substr( 4, 2);
        result += ':';
        result += text.substr( 6, 2);
        result += ':';
        result += text.substr( 8, 2);
        result += ':';
        result += text.substr(10, 2);
        return result;
    }

    static hexToBit(text, iBit)
    {
       let value = parseInt(text, 16);
       value >>= iBit;
       return value & 1;
    }
    static makeIp6(text)
    {
        function makeWord(text)
        {
            let word = parseInt(text, 16);
            if (word === 0) return '';
            return word.toString(16);
        }
        text = text.toLowerCase();
        let result = '';
        result += makeWord(text.substr( 0, 4));
        result += ':';
        result += makeWord(text.substr( 4, 4));
        result += ':';
        result += makeWord(text.substr( 8, 4));
        result += ':';
        result += makeWord(text.substr(12, 4));
        result += ':';
        result += makeWord(text.substr(16, 4));
        result += ':';
        result += makeWord(text.substr(20, 4));
        result += ':';
        result += makeWord(text.substr(24, 4));
        result += ':';
        result += makeWord(text.substr(28, 4));
        return result;
    }
}