#include <stdint.h>
#include <stdbool.h>

struct HttpConnection
{
    int      id; //An id of zero means the record is empty
    uint32_t lastUsed;
    int      toDo;
    bool     postComplete;
    uint32_t delayUntil;
};

extern struct HttpConnection* HttpConnectionNew   (int connectionId); //Never fails so never returns NULL
extern struct HttpConnection* HttpConnectionOrNull(int connectionId);
extern void                   HttpConnectionReset (int connectionId);