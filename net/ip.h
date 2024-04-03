
extern void IpProtocolString(uint8_t protocol, int size, char* text);
extern void IpProtocolLog   (uint8_t protocol);

#define  HOPOPT 0
#define   ICMP  1
#define   IGMP  2
#define    TCP  6
#define    UDP 17
#define IP6IN4 41
#define  ICMP6 58
