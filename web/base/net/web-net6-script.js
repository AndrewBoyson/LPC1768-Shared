//Net6 script
'use strict';

let arp             = '';

let hopLimit        = '';
let mtu             = '';
let managed         = false;
let other           = false;
let routerMac       = '';
let dnsLife         = '';
let dnsIp           = '';
let ndpLease        = '';
let ndpElapsed      = '';
let slaac           = '';

let uniquePrefixLength    = '';
let uniquePrefixL         = false;
let uniquePrefixA         = false;
let uniquePrefixLimit     = '';
let uniquePrefixPreferred = '';
let uniquePrefix          = '';
let globalPrefixLength    = '';
let globalPrefixL         = false;
let globalPrefixA         = false;
let globalPrefixLimit     = '';
let globalPrefixPreferred = '';
let globalPrefix          = '';

function parseArpLine(line)
{
    if (line.length == 0) return;
    let fields = line.split('\t');
    
    arp += parseInt(fields[0], 16).toString().padStart(2, ' ');
    
    let minutes  = parseInt(fields[1], 16) / 1000 / 60;
    arp += Math.floor(minutes).toString().padStart(4, ' ');
    
    arp += ' ';
    arp += Net.makeIp6(fields[2]).padEnd(40, ' ');
    
    arp += ' ';
    let mac = Net.makeMac(fields[3]);
    arp += mac;
    
    let vendor = Net.getVendorFromLocalStorage(mac);
    if (vendor)
    {
        arp += ' ';
        arp += vendor;
    }
    else
    {
        Net.addVendorToLocalStorage(mac);
    }
    
    arp += '\r\n';
}
function parseArpLines(text)
{
    arp = '';
    text.split('\n').forEach(parseArpLine);
}
function parseGenLines(text)
{
    let lines = text.split('\n');
    
    slaac                 = Net.makeIp6 (lines[ 0]    );
    
    hopLimit              =     parseInt(lines[ 1], 16);
    mtu                   =     parseInt(lines[ 2], 16);
    managed               = Net.hexToBit(lines[ 3],  0);
    other                 = Net.hexToBit(lines[ 3],  1);
    routerMac             = Net.makeMac (lines[ 4], 16);
    ndpLease              =     parseInt(lines[ 5], 16);
    ndpElapsed            =     parseInt(lines[ 6], 16);
    
    dnsIp                 = Net.makeIp6 (lines[ 7]    );
    dnsLife               =     parseInt(lines[ 8], 16);
    
    uniquePrefixL         = Net.hexToBit(lines[ 9],  0);
    uniquePrefixA         = Net.hexToBit(lines[ 9],  1);
    uniquePrefixLength    =     parseInt(lines[10], 16);
    uniquePrefix          = Net.makeIp6 (lines[11]    );
    uniquePrefixLimit     =     parseInt(lines[12], 16);
    uniquePrefixPreferred =     parseInt(lines[13], 16);
    
    globalPrefixL         = Net.hexToBit(lines[14],  0);
    globalPrefixA         = Net.hexToBit(lines[14],  1);
    globalPrefixLength    =     parseInt(lines[15], 16);
    globalPrefix          = Net.makeIp6 (lines[16]    );
    globalPrefixLimit     =     parseInt(lines[17], 16);
    globalPrefixPreferred =     parseInt(lines[18], 16);
}
function parse()
{
    let topics = Ajax.response.split('\f');
    parseGenLines(topics[0]);
    parseArpLines(topics[1]);
}
function display()
{
    let elem;
    
    elem = Ajax.getElementOrNull('ajax-arp'             ); if (elem) elem.textContent = arp;
    elem = Ajax.getElementOrNull('ajax-slaac'           ); if (elem) elem.textContent = slaac;
    elem = Ajax.getElementOrNull('ajax-hop-limit'       ); if (elem) elem.textContent = hopLimit;
    elem = Ajax.getElementOrNull('ajax-mtu'             ); if (elem) elem.textContent = mtu;
    elem = Ajax.getElementOrNull('ajax-managed'         ); if (elem) elem.setAttribute('dir', managed ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-other'           ); if (elem) elem.setAttribute('dir', other   ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-router-mac'      ); if (elem) elem.textContent = routerMac;
    elem = Ajax.getElementOrNull('ajax-ndp-lease'       ); if (elem) elem.textContent = ndpLease;
    elem = Ajax.getElementOrNull('ajax-ndp-elapsed'     ); if (elem) elem.textContent = ndpElapsed;
    elem = Ajax.getElementOrNull('ajax-dns-life'        ); if (elem) elem.textContent = dnsLife;
    elem = Ajax.getElementOrNull('ajax-dns-ip'          ); if (elem) elem.textContent = dnsIp;
    
    elem = Ajax.getElementOrNull('ajax-unique-prefix-l'        ); if (elem) elem.setAttribute('dir', uniquePrefixL ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-unique-prefix-a'        ); if (elem) elem.setAttribute('dir', uniquePrefixA ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-unique-prefix-length'   ); if (elem) elem.textContent = uniquePrefixLength;
    elem = Ajax.getElementOrNull('ajax-unique-prefix'          ); if (elem) elem.textContent = uniquePrefix;
    elem = Ajax.getElementOrNull('ajax-unique-prefix-limit'    ); if (elem) elem.textContent = uniquePrefixLimit;
    elem = Ajax.getElementOrNull('ajax-unique-prefix-preferred'); if (elem) elem.textContent = uniquePrefixPreferred;
    
    elem = Ajax.getElementOrNull('ajax-global-prefix-l'        ); if (elem) elem.setAttribute('dir', globalPrefixL ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-global-prefix-a'        ); if (elem) elem.setAttribute('dir', globalPrefixA ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-global-prefix-length'   ); if (elem) elem.textContent = globalPrefixLength;
    elem = Ajax.getElementOrNull('ajax-global-prefix'          ); if (elem) elem.textContent = globalPrefix;
    elem = Ajax.getElementOrNull('ajax-global-prefix-limit'    ); if (elem) elem.textContent = globalPrefixLimit;
    elem = Ajax.getElementOrNull('ajax-global-prefix-preferred'); if (elem) elem.textContent = globalPrefixPreferred;
}

Ajax.server     = '/net6-ajax';
Ajax.onResponse = function() { parse(); display(); };
Ajax.init();