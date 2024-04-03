#include <stdio.h>
#include <stdlib.h>

static int hexToInt(char c)
{
    int nibble = 0;
    if (c >= '0' && c <= '9') nibble = c - '0';
    if (c >= 'A' && c <= 'F') nibble = c - 'A' + 0xA;
    if (c >= 'a' && c <= 'f') nibble = c - 'a' + 0xA;
    return nibble;
}
void HttpQueryUnencode(char* pValue)
{
    if (!pValue) return;
    char* pDst = pValue;
    for (char* pSrc = pValue; *pSrc; pSrc++)
    {
        char c = *pSrc;
        switch (c)
        {
            case '+':
                c = ' ';
                break;
            case '%':
			{
                c = *++pSrc;
                if (c == 0) break;
                int a = hexToInt(c);
                a <<= 4;
                c = *++pSrc;
                if (c == 0) break;
                a += hexToInt(c);
                c = a;
                break;
			}
            default:
                c = *pSrc;
                break;
        }
        *pDst++ = c;
    }
    *pDst = 0;
}
char* HttpQuerySplit(char* p, char** ppName, char** ppValue) //returns the start of the next name value pair
{    
    *ppName    = p;                     //Record the start of the name
    *ppValue   = NULL;

    while (*p != '=')                   //Loop to an '='
    {
        if (*p == 0)    return 0;
        p++;
    }
    *p = 0;                             //Terminate the name by replacing the '=' with a NUL char
    p++;                                //Move on to the start of the value
    *ppValue = p;                       //Record the start of the value
    while (*p != '&')                   //Loop to a '&'
    {
        if (*p == 0)    return 0;
        p++;
    }
    *p = 0;                            //Terminate the value by replacing the '&' with a NULL
    return p + 1;
}
int HttpQueryValueAsInt(char* pValue)
{
    if (pValue) return (int)strtol(pValue, NULL, 10);
    return 0;
}
double HttpQueryValueAsDouble(char* pValue)
{
    return strtod(pValue, NULL);
}
char* HttpCookiesSplit(char* p, char** ppName, char** ppValue) //returns the start of the next name value pair
{    
    *ppValue   = NULL;
    *ppName    = NULL;

    *ppName    = p;                     //Record the start of the name
    while (*p != '=')                   //Loop to an '='
    {
        if (*p == 0) return 0;
        p++;
    }
    *p = 0;                             //Terminate the name by replacing the '=' with a NUL char
    p++;                                //Move on to the start of the value
    *ppValue = p;                       //Record the start of the value
    while (*p != ';')                   //Loop to a ';'
    {
        if (*p == 0) return 0;
        p++;
    }
    *p = 0;                            //Terminate the value by replacing the ';' with a NULL
    p++;
    if (*p == 0)     return 0;
    while (*p == ' ') p++;             //Move past any spaces after the ';'
    return p;
}
