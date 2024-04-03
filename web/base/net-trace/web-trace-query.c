#include "web/http/http.h"
#include "settings/settings.h"

void WebTraceQuery(char* pQuery)
{
    while (pQuery)
    {
        char* pName;
        char* pValue;
        pQuery = HttpQuerySplit(pQuery, &pName, &pValue);
        
        if (HttpSameStr(pName, "chg-send-dns-ip4"      )) ChgDnsSendRequestsViaIp4();
        if (HttpSameStr(pName, "chg-send-ntp-ip4"      )) ChgNtpSendRequestsViaIp4();
        if (HttpSameStr(pName, "chg-send-tftp-ip4"     )) ChgTftpSendRequestsViaIp4();
        if (HttpSameStr(pName, "set-trace-net-host"    )) SetTraceNetHost(pValue);                    
        if (HttpSameStr(pName, "chg-trace-net-stack"   )) ChgTraceNetStack();
        if (HttpSameStr(pName, "chg-trace-net-newline" )) ChgTraceNetNewLine();
        if (HttpSameStr(pName, "chg-trace-net-verbose" )) ChgTraceNetVerbose();
        if (HttpSameStr(pName, "chg-trace-link"        )) ChgTraceLink();
        if (HttpSameStr(pName, "chg-trace-dns-name"    )) ChgTraceDnsName();
        if (HttpSameStr(pName, "chg-trace-dns-query"   )) ChgTraceDnsQuery();
        if (HttpSameStr(pName, "chg-trace-dns-reply"   )) ChgTraceDnsReply();
        if (HttpSameStr(pName, "chg-trace-dns-server"  )) ChgTraceDnsServer();
        if (HttpSameStr(pName, "chg-trace-ntp"         )) ChgTraceNtp();
        if (HttpSameStr(pName, "chg-trace-dhcp"        )) ChgTraceDhcp();
        if (HttpSameStr(pName, "chg-trace-ns-recv-sol" )) ChgTraceNsRecvSol();
        if (HttpSameStr(pName, "chg-trace-ns-recv-adv" )) ChgTraceNsRecvAdv();
        if (HttpSameStr(pName, "chg-trace-ns-send-sol" )) ChgTraceNsSendSol();
        if (HttpSameStr(pName, "chg-trace-nr"          )) ChgTraceNr();
        if (HttpSameStr(pName, "chg-trace-ntp-client"  )) ChgTraceNtpClient();
        if (HttpSameStr(pName, "chg-trace-sync"        )) ChgTraceSync();
        if (HttpSameStr(pName, "chg-trace-echo4"       )) ChgTraceEcho4();
        if (HttpSameStr(pName, "chg-trace-echo6"       )) ChgTraceEcho6();
        if (HttpSameStr(pName, "chg-trace-dest6"       )) ChgTraceDest6();
        if (HttpSameStr(pName, "chg-trace-ra"          )) ChgTraceRa();
        if (HttpSameStr(pName, "chg-trace-rs"          )) ChgTraceRs();
        if (HttpSameStr(pName, "chg-trace-ar4"         )) ChgTraceAr4();
        if (HttpSameStr(pName, "chg-trace-ar6"         )) ChgTraceAr6();
        if (HttpSameStr(pName, "chg-trace-arp"         )) ChgTraceArp();
        if (HttpSameStr(pName, "chg-trace-ip4"         )) ChgTraceIp4();
        if (HttpSameStr(pName, "chg-trace-ip6"         )) ChgTraceIp6();
        if (HttpSameStr(pName, "chg-trace-udp"         )) ChgTraceUdp();
        if (HttpSameStr(pName, "chg-trace-tcp"         )) ChgTraceTcp();
        if (HttpSameStr(pName, "chg-trace-http"        )) ChgTraceHttp();
        if (HttpSameStr(pName, "chg-trace-tftp"        )) ChgTraceTftp();
    }
}

