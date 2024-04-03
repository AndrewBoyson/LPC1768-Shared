//Net script
'use strict';

let mac = '';
let nr  = '';
let tcp = '';
let dns = '';
let a4s = null;
let a6s = null;

function parseTcpLine(line)
{
    if (line.length == 0) return;
    let fields = line.split('\t');
    let state      = parseInt(fields[0], 16);
    let idleMs     = parseInt(fields[1], 16);
    let ipType     = parseInt(fields[2], 16);
    let arIndex    = parseInt(fields[3], 16);
    let locPort    = parseInt(fields[4], 16);
    let remPort    = parseInt(fields[5], 16);
    let bytesRcvd  = parseInt(fields[6], 16);
    let bytesSent  = parseInt(fields[7], 16);
    
    switch (state)
    {
        case 1:  tcp += "  Syn";                           break;
        case 2:  tcp += "  Est";                           break;
        case 3:  tcp += " Wait";                           break;
        default: tcp += state.toString().padStart(5, ' '); break;
    }
    
    let idleMinutes = Math.floor(idleMs / 1000 / 60);
    tcp += idleMinutes.toString().padStart(4, ' ');
    tcp += "     ";
    
    if      (ipType == 0x0800) tcp += "IPv4";
    else if (ipType == 0x86DD) tcp += "IPv6";
    else                       tcp += fields[1];
    
    tcp +=   arIndex.toString().padStart(4, ' ');
    tcp +=   locPort.toString().padStart(9, ' ');
    tcp +=   remPort.toString().padStart(9, ' ');
    tcp += bytesRcvd.toString().padStart(6, ' ');
    tcp += bytesSent.toString().padStart(6, ' ');
    tcp += '\r\n';
}
function parseDnsLine(line)
{
    if (line.length == 0) return;
    let fields = line.split('\t');
    
    dns += parseInt(fields[0], 16).toString().padStart(2, ' ');
    let minutes  = parseInt(fields[1], 16) / 1000 / 60;
    dns += Math.floor(minutes).toString().padStart(4, ' ');
    dns += ' ';
    let addrType = fields[2];
    switch (addrType)
    {
        case '4': dns += '   A'; break;
        case '6': dns += 'AAAA'; break;
        default:  dns += addrType; break;
    }
    dns += ' ';
    let addr = '';
    let mac = '';
    switch (addrType)
    {
        case '4':
            if (fields[3] === '00000000') break;
            addr = Net.makeIp4(fields[3]);
            if (a4s.has(addr)) mac = a4s.get(addr);
            break;
        case '6':
            if (fields[3].substr(0, 2) === '00') break;
            addr = Net.makeIp6(fields[3]);
            if (a6s.has(addr)) mac = a6s.get(addr);
            break;
    }
    dns += addr.padEnd(40, ' ');
    
    dns += ' ';
    dns += fields[4]; //DNS protocol used
    dns += ' ';
    dns += fields[5].padEnd(32, ' '); //Resolved name
    
    if (mac != '')
    {
        dns += ' ';
        dns += mac;
        dns += ' ';
        
        let vendor = Net.getVendorFromLocalStorage(mac);
        if (vendor)
        {
            dns += ' ';
            dns += vendor;
        }
        else
        {
            Net.addVendorToLocalStorage(mac);
        }
    }
    dns += '\r\n';
}
function parseAr4Line(line)
{
    if (line.length == 0) return;
    let fields = line.split('\t');
    
    let index =    parseInt(fields[0], 16);
    let ms    =    parseInt(fields[1], 16);
    let addr  = Net.makeIp4(fields[2]);
    let mac   = Net.makeMac(fields[3]);
    
    a4s.set(addr, mac);
}
function parseAr6Line(line)
{
    if (line.length == 0) return;
    let fields = line.split('\t');
    
    let index =    parseInt(fields[0], 16);
    let ms    =    parseInt(fields[1], 16);
    let addr  = Net.makeIp6(fields[2]);
    let mac   = Net.makeMac(fields[3]);
    
    a6s.set(addr, mac);
}
function parseGenLines(text)
{
    let lines = text.split('\n');
    mac = Net.makeMac(lines[ 0]);
    nr  =             lines[ 1] ;
}
function parseTcpLines(text)
{
    tcp = "State Idle Protocol ARI Port-Loc Port-Rem  Rcvd  Sent\n";
    text.split('\n').forEach(parseTcpLine);
}
function parseDnsLines(text)
{
    dns = '';
    text.split('\n').forEach(parseDnsLine);
}
function parseAr4Lines(text)
{
    a4s = new Map();
    text.split('\n').forEach(parseAr4Line);
}
function parseAr6Lines(text)
{
    a6s = new Map();
    text.split('\n').forEach(parseAr6Line);
}
function parse()
{
    let topics = Ajax.response.split('\f');
    parseGenLines(topics[0]);
    parseTcpLines(topics[1]);
    parseAr4Lines(topics[3]);
    parseAr6Lines(topics[4]);
    parseDnsLines(topics[2]);
}
function display()
{
    let elem;

    elem = Ajax.getElementOrNull('ajax-tcp'         ); if (elem) elem.textContent = tcp;
    elem = Ajax.getElementOrNull('ajax-mac'         ); if (elem) elem.textContent = mac;
    elem = Ajax.getElementOrNull('ajax-nr'          ); if (elem) elem.value       = nr;
    elem = Ajax.getElementOrNull('ajax-dns'         ); if (elem) elem.textContent = dns;
}

Ajax.server     = '/net-ajax';
Ajax.onResponse = function() { parse(); display(); };
Ajax.init();
