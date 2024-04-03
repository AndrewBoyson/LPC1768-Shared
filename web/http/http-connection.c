#include <stdlib.h>

#include "http-connection.h"
#include "lpc1768/mstimer/mstimer.h"

#define MAX_CONNECTIONS 10

static struct HttpConnection connections[MAX_CONNECTIONS];

static void zeroConnection(struct HttpConnection* p)
{
    p->id           = 0;
    p->lastUsed     = 0;
    p->toDo         = 0;
    p->postComplete = false;
    p->delayUntil   = 0;
}

struct HttpConnection* HttpConnectionNew(int connectionId) //Never fails so never returns NULL
{
    struct HttpConnection* p;
    
    //Look for an existing connection
    for (p = connections; p < connections + MAX_CONNECTIONS; p++)
    {
        if (p->id == connectionId) goto end;
    }
    
    //look for an empty connection
    {
        struct HttpConnection* pOldest = 0;
        uint32_t ageOldest = 0;
        for (p = connections; p < connections + MAX_CONNECTIONS; p++)
        {
            if (!p->id) goto end;
            
            //Otherwise record the oldest and keep going
            uint32_t age = MsTimerCount - p->lastUsed;
            if (age >= ageOldest)
            {
                ageOldest = age;
                  pOldest = p;
            }
        }
    
        //No empty ones found so use the oldest
        p = pOldest;
    }
    
end:
    zeroConnection(p);
    p->id           = connectionId;
    p->lastUsed     = MsTimerCount;
    return p;
}
struct HttpConnection* HttpConnectionOrNull(int connectionId)
{
    for (struct HttpConnection* p = connections; p < connections + MAX_CONNECTIONS; p++)
    {
        if (p->id == connectionId)
        {
            p->lastUsed = MsTimerCount;
            return p;
        }
    }
    return NULL;
}
void HttpConnectionReset(int connectionId)
{
    for (struct HttpConnection* p = connections; p < connections + MAX_CONNECTIONS; p++)
    {
        if (p->id == connectionId) zeroConnection(p);
    }
}
