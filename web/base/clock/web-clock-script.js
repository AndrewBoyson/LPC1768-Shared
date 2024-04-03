//Clock script
'use strict';

let pseudo          = new Clock();
let rtc             = new Clock();

let pseudoDisplay   = false;
let pseudoStartMs   = 0;

let diffMs          = 0;
let rtcIsSet        = false;
let clockIsSet      = false;
let sourceIsOk      = false;
let rateIsLocked    = false;
let timeIsLocked    = false;

let ppb             = 0;
let ppbdivisor      = 0;
let ppbmaxchange    = 0;
let syncedlimitppb  = 0;
let syncedhysppb    = 0;
let slewdivisor     = 0;
let slewmax         = 0;
let syncedlimitns   = 0;
let syncedhysns     = 0;
let maxoffsetsecs   = 0;
let govTrace        = false;

let ntpserver       = '';
let ntpinitial      = 0;
let ntpnormal       = 0;
let ntpretry        = 0;
let ntpoffset       = 0;
let ntpmaxdelay     = 0;

let scanavg         = 0;
let scanmax         = 0;
let scanmin         = 0;

const DISPLAY_LEAP_MS = 10000;

function parseLinesTime(text)
{
    let lines = text.split('\n');
    rtc.ms          = Ajax.date.getTime();
    rtc.ms         += parseInt(lines[0], 16);
    rtc.ms         -= Ajax.ms;
    diffMs          = rtc.ms + Ajax.ms - Date.now();
    rtcIsSet        = Ajax.hexToBit(lines[1], 0);
    clockIsSet      = Ajax.hexToBit(lines[1], 1);
    sourceIsOk      = Ajax.hexToBit(lines[1], 2);
    rateIsLocked    = Ajax.hexToBit(lines[1], 3);
    timeIsLocked    = Ajax.hexToBit(lines[1], 4);
    rtc.leapEnable  = Ajax.hexToBit(lines[1], 5);
    rtc.leapForward = Ajax.hexToBit(lines[1], 6);
    govTrace        = Ajax.hexToBit(lines[1], 7);
    rtc.months1970  = parseInt(lines[2], 16);
    rtc.leaps       = parseInt(lines[3], 16);
}
function parseLinesGov(text)
{
    let lines = text.split('\n');
    ppb             = parseInt(lines[0], 16);
    ppbdivisor      = parseInt(lines[1], 16);
    ppbmaxchange    = parseInt(lines[2], 16);
    syncedlimitppb  = parseInt(lines[3], 16);
    syncedhysppb    = parseInt(lines[4], 16);
    slewdivisor     = parseInt(lines[5], 16);
    slewmax         = parseInt(lines[6], 16);
    syncedlimitns   = parseInt(lines[7], 16);
    syncedhysns     = parseInt(lines[8], 16);
    maxoffsetsecs   = parseInt(lines[9], 16);
}
function parseLinesNtp(text)
{
    let lines = text.split('\n');
    ntpserver       =          lines[0];
    ntpinitial      = parseInt(lines[1], 16);
    ntpnormal       = parseInt(lines[2], 16);
    ntpretry        = parseInt(lines[3], 16);
    ntpoffset       = parseInt(lines[4], 16);
    ntpmaxdelay     = parseInt(lines[5], 16);
}
function parseLinesScan(text)
{
    let lines = text.split('\n');
    scanavg         = parseInt(lines[0], 16);
    scanmax         = parseInt(lines[1], 16);
    scanmin         = parseInt(lines[2], 16);
}
function parse()
{
    let topics = Ajax.response.split('\f');
    parseLinesTime(topics[0]);
    parseLinesGov (topics[1]);
    parseLinesNtp (topics[2]);
    parseLinesScan(topics[3]);
}
function display()
{
    let elem;
    elem = Ajax.getElementOrNull('ajax-rtc-set'      ); if (elem) elem.setAttribute('dir', rtcIsSet     ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-clock-set'    ); if (elem) elem.setAttribute('dir', clockIsSet   ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-source-ok'    ); if (elem) elem.setAttribute('dir', sourceIsOk   ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-rate-locked'  ); if (elem) elem.setAttribute('dir', rateIsLocked ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-time-locked'  ); if (elem) elem.setAttribute('dir', timeIsLocked ? 'rtl' : 'ltr');
    
    elem = Ajax.getElementOrNull('ajax-leap-enable'  ); if (elem) elem.setAttribute('dir', rtc.leapEnable   ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-leap-forward' ); if (elem) elem.setAttribute('dir', rtc.leapForward  ? 'rtl' : 'ltr');
    
    elem = Ajax.getElementOrNull('ajax-leap-year'    ); if (elem) elem.value = rtc.months1970 ? rtc.leapYear  : '';
    elem = Ajax.getElementOrNull('ajax-leap-month'   ); if (elem) elem.value = rtc.months1970 ? rtc.leapMonth : '';
    
    elem = Ajax.getElementOrNull('ajax-leap-count'   ); if (elem) elem.value = rtc.leaps;
    
    elem = Ajax.getElementOrNull('ajax-ppb'          ); if (elem) elem.value = ppb;
    elem = Ajax.getElementOrNull('ajax-ppb-divisor'  ); if (elem) elem.value = ppbdivisor;
    elem = Ajax.getElementOrNull('ajax-ppb-max-chg'  ); if (elem) elem.value = ppbmaxchange;
    elem = Ajax.getElementOrNull('ajax-ppb-syn-lim'  ); if (elem) elem.value = syncedlimitppb;
    elem = Ajax.getElementOrNull('ajax-ppb-syn-hys'  ); if (elem) elem.value = syncedhysppb;
    elem = Ajax.getElementOrNull('ajax-off-divisor'  ); if (elem) elem.value = slewdivisor;
    elem = Ajax.getElementOrNull('ajax-off-max'      ); if (elem) elem.value = slewmax;
    elem = Ajax.getElementOrNull('ajax-off-syn-lim'  ); if (elem) elem.value = syncedlimitns / 1000000;
    elem = Ajax.getElementOrNull('ajax-off-syn-hys'  ); if (elem) elem.value = syncedhysns   / 1000000;
    elem = Ajax.getElementOrNull('ajax-off-rst-lim'  ); if (elem) elem.value = maxoffsetsecs;
    elem = Ajax.getElementOrNull('ajax-gov-trace'    ); if (elem) elem.setAttribute('dir', govTrace     ? 'rtl' : 'ltr');
    
    elem = Ajax.getElementOrNull('ajax-ntp-server'   ); if (elem) elem.value = ntpserver;
    elem = Ajax.getElementOrNull('ajax-ntp-initial'  ); if (elem) elem.value = ntpinitial;
    elem = Ajax.getElementOrNull('ajax-ntp-normal'   ); if (elem) elem.value = ntpnormal / 60;
    elem = Ajax.getElementOrNull('ajax-ntp-retry'    ); if (elem) elem.value = ntpretry;
    elem = Ajax.getElementOrNull('ajax-ntp-offset'   ); if (elem) elem.value = ntpoffset;
    elem = Ajax.getElementOrNull('ajax-ntp-max-delay'); if (elem) elem.value = ntpmaxdelay;
    
    elem = Ajax.getElementOrNull('ajax-scan-avg'     ); if (elem) elem.textContent = scanavg;
    elem = Ajax.getElementOrNull('ajax-scan-max'     ); if (elem) elem.textContent = scanmax;
    elem = Ajax.getElementOrNull('ajax-scan-min'     ); if (elem) elem.textContent = scanmin;
    
    elem = Ajax.getElementOrNull('ajax-date-diff'    ); if (elem) elem.textContent = diffMs;
}

function handleTick() //This typically called every 100ms
{
    if (pseudoDisplay)
    {
        pseudo.adjustLeap (Ajax.ms);
        pseudo.displayTime(Ajax.ms);
        if (Ajax.ms >= pseudoStartMs + DISPLAY_LEAP_MS + 500) pseudoDisplay = false;
    }
    else
    {
        rtc.adjustLeap (Ajax.ms);
        rtc.displayTime(Ajax.ms);
    }
}

function displayLeap() //Called by display leap button in HTML
{
   pseudoDisplay = true;
   pseudoStartMs = Ajax.ms;
   
   pseudo.leapEnable  = true;
   pseudo.leapForward = rtc.leapForward;
   pseudo.leaps       = rtc.leaps;
   pseudo.leapMonth   = rtc.leapMonth;
   pseudo.leapYear    = rtc.leapYear;
   pseudo.ms          = Date.UTC(rtc.leapYear, rtc.leapMonth - 1, 1) - DISPLAY_LEAP_MS / 2 - Ajax.ms;
}
Ajax.server     = '/clock-ajax';
Ajax.onResponse = function() { parse(); display(); };
Ajax.onTick     = handleTick;
Ajax.init();