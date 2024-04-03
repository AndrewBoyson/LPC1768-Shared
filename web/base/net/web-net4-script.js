//Net4 script
'use strict';

let localIp     = '';
let domainName  = '';
let hostName    = '';
let ntpIp       = '';
let dnsIp       = '';
let dhcpIp      = '';
let routerIp    = '';
let subnetMask  = '';
let broadcastIp = '';
let leaseTime   = '';
let renewalT1   = '';
let renewalt2   = '';
let elapsed     = '';
let arp         = '';

function parseArpLine(line)
{
    if (line.length == 0) return;
    let fields = line.split('\t');
    
    arp += parseInt(fields[0], 16).toString().padStart(2, ' ');
    
    let minutes  = parseInt(fields[1], 16) / 1000 / 60;
    arp += Math.floor(minutes).toString().padStart(4, ' ');
    
    arp += ' ';
    arp += Net.makeIp4(fields[2]).padEnd(15, ' ');
    
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
    localIp     = Net.makeIp4(lines[ 0])    ;
    domainName  =             lines[ 1]     ;
    hostName    =             lines[ 2]     ;
    ntpIp       = Net.makeIp4(lines[ 3])    ;
    dnsIp       = Net.makeIp4(lines[ 4])    ;
    dhcpIp      = Net.makeIp4(lines[ 5])    ;
    routerIp    = Net.makeIp4(lines[ 6])    ;
    subnetMask  = Net.makeIp4(lines[ 7])    ;
    broadcastIp = Net.makeIp4(lines[ 8])    ;
    leaseTime   =    parseInt(lines[ 9], 16);
    renewalT1   =    parseInt(lines[10], 16);
    renewalt2   =    parseInt(lines[11], 16);
    elapsed     =    parseInt(lines[12], 16);
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

    elem = Ajax.getElementOrNull('ajax-local-ip'    ); if (elem) elem.textContent = localIp;
    elem = Ajax.getElementOrNull('ajax-domain-name' ); if (elem) elem.textContent = domainName;
    elem = Ajax.getElementOrNull('ajax-host-name'   ); if (elem) elem.textContent = hostName;
    elem = Ajax.getElementOrNull('ajax-ntp-ip'      ); if (elem) elem.textContent = ntpIp;
    elem = Ajax.getElementOrNull('ajax-dns-ip'      ); if (elem) elem.textContent = dnsIp;
    elem = Ajax.getElementOrNull('ajax-dhcp-ip'     ); if (elem) elem.textContent = dhcpIp;
    elem = Ajax.getElementOrNull('ajax-router-ip'   ); if (elem) elem.textContent = routerIp;
    elem = Ajax.getElementOrNull('ajax-subnet-mask' ); if (elem) elem.textContent = subnetMask;
    elem = Ajax.getElementOrNull('ajax-broadcast-ip'); if (elem) elem.textContent = broadcastIp;
    elem = Ajax.getElementOrNull('ajax-lease-time'  ); if (elem) elem.textContent = leaseTime;
    elem = Ajax.getElementOrNull('ajax-renewal-t1'  ); if (elem) elem.textContent = renewalT1;
    elem = Ajax.getElementOrNull('ajax-renewal-t2'  ); if (elem) elem.textContent = renewalt2;
    elem = Ajax.getElementOrNull('ajax-elapsed'     ); if (elem) elem.textContent = elapsed;
    elem = Ajax.getElementOrNull('ajax-arp'         ); if (elem) elem.textContent = arp;
}

Ajax.server     = '/net4-ajax';
Ajax.onResponse = function() { parse(); display(); };
Ajax.init();