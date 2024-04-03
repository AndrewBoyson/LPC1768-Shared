#include <stdbool.h>
#include <stdint.h>

#include "web/http/http.h"

#define GPREG2 (*((volatile unsigned *) 0x4002404C))

static uint32_t hash;
static bool hashIsSet = false;

void  WebLoginPasswordRestore()
{
    hash = GPREG2;
    hashIsSet = true;
}

void WebLoginPasswordReset()
{
    GPREG2 = 0;
    hashIsSet = false;
}
uint32_t hasher(char *p) //Jenkins 'one at a time' hash
{
    uint32_t h = 0;
    
    while (*p)
    {
        h += *p;
        h += (h << 10);
        h ^= (h >> 6);
        p++;
    }
    h += (h << 3);
    h ^= (h >> 11);
    h += (h << 15);
        
    return h;
}
bool WebLoginPasswordIsSet()
{
    return hashIsSet;
}
void WebLoginPasswordSet(char* password)
{
    if (!password)  return;
    if (!*password) return;
    hash = hasher(password);
    GPREG2 = hash;
    hashIsSet = true;
}
bool WebLoginPasswordMatches(char* password)
{
    if (!hashIsSet) return false;
    if (!password)  return false;
    if (!*password) return false;
    return hash == hasher(password);
}
