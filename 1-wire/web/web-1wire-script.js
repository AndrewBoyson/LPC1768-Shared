//1wire script
'use strict';

let allRomValues        = new Map();
let oneWireScanTime     = 0;
let oneWireLowTweak     = 0;
let oneWireFloatTweak   = 0;
let oneWireReadTweak    = 0;
let oneWireHighTweak    = 0;
let oneWireReleaseTweak = 0;
let oneWireTrace        = false;
let assignedRoms        = new Array();
let assignedNames       = new Array();

function getAssignedName(rom)
{
    for (let i = 0; i < assignedRoms.length; i++)
    {
        if (assignedRoms[i] === rom) return assignedNames[i]; 
    }
    return '';
}

function addRomValue(line)
{
    if (line) allRomValues.set(line.substr(0, 16), line.substr(16));
}
function parseAllRomValues(topic)
{
    allRomValues.clear();
    topic.split('\n').forEach(addRomValue);
}
function parseTimings(topic)
{
    let lines = topic.split('\n');
    oneWireScanTime     = Ajax.hexToSignedInt16(lines[0]);
    oneWireLowTweak     = Ajax.hexToSignedInt16(lines[1]);
    oneWireFloatTweak   = Ajax.hexToSignedInt16(lines[2]);
    oneWireReadTweak    = Ajax.hexToSignedInt16(lines[3]);
    oneWireHighTweak    = Ajax.hexToSignedInt16(lines[4]);
    oneWireReleaseTweak = Ajax.hexToSignedInt16(lines[5]);
    oneWireTrace        =                       lines[6] != '0';
}
function addRom(line)
{
    if (line)
    {
        assignedRoms.push(line.substr(0, 16))
        assignedNames.push(line.substr(16));
    }
}
function parseAssigned(topic)
{
    assignedRoms  = new Array();
    assignedNames = new Array();
    topic.split('\n').forEach(addRom);
}
function parse()
{
    let topics = Ajax.response.split('\f');
    parseTimings     (topics[0]);
    parseAllRomValues(topics[1]);
    parseAssigned    (topics[2]);
}
function display()
{
    let elem;
    elem = Ajax.getElementOrNull('ajax-device-values');
    if (elem) 
    {
        elem.textContent = '';
        for (let [key, value] of allRomValues)
        {
            elem.textContent += key;
            elem.textContent += ' '; 
            elem.textContent += OneWire.DS18B20ToString(Ajax.hexToSignedInt16(value));
            elem.textContent += ' ';
            elem.textContent += getAssignedName(key);
            elem.textContent += '\r\n';
        }
    }
    elem = Ajax.getElementOrNull('ajax-1-wire-scan-time'    ); if (elem) elem.textContent = oneWireScanTime;
    elem = Ajax.getElementOrNull('ajax-1-wire-tweak-low'    ); if (elem) elem.textContent = oneWireLowTweak;
    elem = Ajax.getElementOrNull('ajax-1-wire-tweak-float'  ); if (elem) elem.textContent = oneWireFloatTweak;
    elem = Ajax.getElementOrNull('ajax-1-wire-tweak-read'   ); if (elem) elem.textContent = oneWireReadTweak;
    elem = Ajax.getElementOrNull('ajax-1-wire-tweak-high'   ); if (elem) elem.textContent = oneWireHighTweak;
    elem = Ajax.getElementOrNull('ajax-1-wire-tweak-release'); if (elem) elem.textContent = oneWireReleaseTweak;
    elem = Ajax.getElementOrNull('ajax-1-wire-trace'        ); if (elem) elem.setAttribute('dir', oneWireTrace ? 'rtl' : 'ltr');

    elem = Ajax.getElementOrNull('ajax-name-0'); if (elem && assignedNames.length > 0) elem.textContent = assignedNames[0];
    elem = Ajax.getElementOrNull('ajax-name-1'); if (elem && assignedNames.length > 1) elem.textContent = assignedNames[1];
    elem = Ajax.getElementOrNull('ajax-name-2'); if (elem && assignedNames.length > 2) elem.textContent = assignedNames[2];
    elem = Ajax.getElementOrNull('ajax-name-3'); if (elem && assignedNames.length > 3) elem.textContent = assignedNames[3];
    elem = Ajax.getElementOrNull('ajax-name-4'); if (elem && assignedNames.length > 4) elem.textContent = assignedNames[4];
    elem = Ajax.getElementOrNull('ajax-name-5'); if (elem && assignedNames.length > 5) elem.textContent = assignedNames[5];
    elem = Ajax.getElementOrNull('ajax-name-6'); if (elem && assignedNames.length > 6) elem.textContent = assignedNames[6];
    elem = Ajax.getElementOrNull('ajax-name-7'); if (elem && assignedNames.length > 7) elem.textContent = assignedNames[7];
    elem = Ajax.getElementOrNull('ajax-rom-0' ); if (elem && assignedRoms.length  > 0) elem.value = assignedRoms[0];
    elem = Ajax.getElementOrNull('ajax-rom-1' ); if (elem && assignedRoms.length  > 1) elem.value = assignedRoms[1];
    elem = Ajax.getElementOrNull('ajax-rom-2' ); if (elem && assignedRoms.length  > 2) elem.value = assignedRoms[2];
    elem = Ajax.getElementOrNull('ajax-rom-3' ); if (elem && assignedRoms.length  > 3) elem.value = assignedRoms[3];
    elem = Ajax.getElementOrNull('ajax-rom-4' ); if (elem && assignedRoms.length  > 4) elem.value = assignedRoms[4];
    elem = Ajax.getElementOrNull('ajax-rom-5' ); if (elem && assignedRoms.length  > 5) elem.value = assignedRoms[5];
    elem = Ajax.getElementOrNull('ajax-rom-6' ); if (elem && assignedRoms.length  > 6) elem.value = assignedRoms[6];
    elem = Ajax.getElementOrNull('ajax-rom-7' ); if (elem && assignedRoms.length  > 7) elem.value = assignedRoms[7];
}

Ajax.server     = '/1wire-ajax';
Ajax.onResponse = function() { parse(); display(); };
Ajax.init();
