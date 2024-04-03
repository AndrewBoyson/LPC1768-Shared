#include <stdbool.h>

extern int  ActionMakeFromDestAndTrace(int dest, bool trace);
extern int  ActionGetDestPart         (int action);
extern bool ActionGetTracePart        (int action);

#define DO_NOTHING       0
#define   UNICAST        1
#define   UNICAST_DNS    2
#define   UNICAST_DHCP   3
#define   UNICAST_NTP    4
#define   UNICAST_TFTP   5
#define   UNICAST_USER   6
#define MULTICAST_NODE   7
#define MULTICAST_ROUTER 8
#define MULTICAST_MDNS   9
#define MULTICAST_LLMNR 10
#define MULTICAST_NTP   11
#define SOLICITED_NODE  12
#define BROADCAST       13

/*
If DO_NOTHING then no other flags may be set.
This allows action to be checked for DO_NOTHING without stripping the flags.
Flags must be stripped before using the destination part.
*/