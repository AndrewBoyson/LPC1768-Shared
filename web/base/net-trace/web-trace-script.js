//Net trace script
'use strict';
function setDirection(elem, iChar, iBit)
{
    elem.setAttribute('dir', Ajax.hexToBit(Ajax.response.charAt(iChar), iBit) ? 'rtl' : 'ltr');
}
function display()
{
   var elem;
   elem = Ajax.getElementOrNull('ajax-trace-dns-ip4'     ); if (elem) setDirection(elem,  0, 1);
   elem = Ajax.getElementOrNull('ajax-trace-ntp-ip4'     ); if (elem) setDirection(elem,  0, 2);
   elem = Ajax.getElementOrNull('ajax-trace-tftp-ip4'    ); if (elem) setDirection(elem,  0, 3);
   elem = Ajax.getElementOrNull('ajax-trace-net-host'    ); if (elem) elem.value = Ajax.response.substr( 1, 4);
   elem = Ajax.getElementOrNull('ajax-trace-net-stack'   ); if (elem) setDirection(elem,  5, 0);
   elem = Ajax.getElementOrNull('ajax-trace-net-newline' ); if (elem) setDirection(elem,  5, 1);
   elem = Ajax.getElementOrNull('ajax-trace-net-verbose' ); if (elem) setDirection(elem,  5, 2);
   elem = Ajax.getElementOrNull('ajax-trace-link'        ); if (elem) setDirection(elem,  5, 3);
   elem = Ajax.getElementOrNull('ajax-trace-dns-name'    ); if (elem) setDirection(elem,  6, 0);
   elem = Ajax.getElementOrNull('ajax-trace-dns-query'   ); if (elem) setDirection(elem,  6, 1);
   elem = Ajax.getElementOrNull('ajax-trace-dns-reply'   ); if (elem) setDirection(elem,  6, 2);
   elem = Ajax.getElementOrNull('ajax-trace-dns-server'  ); if (elem) setDirection(elem,  6, 3);
   elem = Ajax.getElementOrNull('ajax-trace-ntp'         ); if (elem) setDirection(elem,  7, 0);
   elem = Ajax.getElementOrNull('ajax-trace-dhcp'        ); if (elem) setDirection(elem,  7, 1);
   elem = Ajax.getElementOrNull('ajax-trace-ns-recv-sol' ); if (elem) setDirection(elem,  7, 2);
   elem = Ajax.getElementOrNull('ajax-trace-ns-recv-adv' ); if (elem) setDirection(elem,  7, 3);
   elem = Ajax.getElementOrNull('ajax-trace-ns-send-sol' ); if (elem) setDirection(elem,  8, 0);
   elem = Ajax.getElementOrNull('ajax-trace-nr'          ); if (elem) setDirection(elem,  8, 2);
   elem = Ajax.getElementOrNull('ajax-trace-ntp-client'  ); if (elem) setDirection(elem,  8, 3);
   elem = Ajax.getElementOrNull('ajax-trace-echo4'       ); if (elem) setDirection(elem,  9, 2);
   elem = Ajax.getElementOrNull('ajax-trace-echo6'       ); if (elem) setDirection(elem,  9, 3);
   elem = Ajax.getElementOrNull('ajax-trace-dest6'       ); if (elem) setDirection(elem, 10, 0);
   elem = Ajax.getElementOrNull('ajax-trace-ra'          ); if (elem) setDirection(elem, 10, 1);
   elem = Ajax.getElementOrNull('ajax-trace-rs'          ); if (elem) setDirection(elem, 10, 2);
   elem = Ajax.getElementOrNull('ajax-trace-ar4'         ); if (elem) setDirection(elem, 10, 3);
   elem = Ajax.getElementOrNull('ajax-trace-ar6'         ); if (elem) setDirection(elem, 11, 0);
   elem = Ajax.getElementOrNull('ajax-trace-arp'         ); if (elem) setDirection(elem, 11, 1);
   elem = Ajax.getElementOrNull('ajax-trace-ip4'         ); if (elem) setDirection(elem, 11, 2);
   elem = Ajax.getElementOrNull('ajax-trace-ip6'         ); if (elem) setDirection(elem, 11, 3);
   elem = Ajax.getElementOrNull('ajax-trace-udp'         ); if (elem) setDirection(elem, 12, 0);
   elem = Ajax.getElementOrNull('ajax-trace-tcp'         ); if (elem) setDirection(elem, 12, 1);
   elem = Ajax.getElementOrNull('ajax-trace-http'        ); if (elem) setDirection(elem, 12, 2);
   elem = Ajax.getElementOrNull('ajax-trace-tftp'        ); if (elem) setDirection(elem, 12, 3);
}

Ajax.server     = '/trace-ajax';
Ajax.onResponse = display;
Ajax.init();