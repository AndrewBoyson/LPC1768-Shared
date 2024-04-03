extern void DnsLabelCopy(char* dst, char* src);
extern bool DnsLabelIsExternal(char* p);
extern bool DnsLabelIsSame(char* pA, char* pB);
extern int  DnsLabelMakeFullNameFromName (int protocol, const char* name, int size, char* fqdn);
extern int  DnsLabelStripNameFromFullName(int protocol, char* fqdn, int size, char* name);

#define DNS_MAX_LABEL_LENGTH 63

/*
rfc1035/rfc1123 label (DNS_LABEL):
An alphanumeric (a-z, and 0-9) string, with a maximum length of 63 characters,
with the '-' character allowed anywhere except the first or last character,
suitable for use as a hostname or segment in a domain name.

Labels are stored in DNS as <length><label>, length is a single byte
two high bits of the length field reserved for something else (compression) thus leaving 6 bits for the length itself
2^6=64 possible values - 0..63.

In our case reserve 64 bytes for all labels which leaves room for a terminating null
so always delare labels, hostnames or segments as [DNS_MAX_LABEL_LENGTH+1].
*/