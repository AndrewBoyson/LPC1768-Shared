#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "http.h"
#include "log/log.h"

static char* terminateThisAndGetNextLine(char* p, char* pE) //Terminates this line and returns the start of the next line or NULL if none
{
    while (true)
    {
        if ( p == pE)   { *p = 0; return NULL;  } //no more buffer - relies on having designed the buffer to be bigger than an ethernet packet
        if (*p == 0)              return NULL;    //there are no more lines
        if (*p == '\n') { *p = 0; return p + 1; } //return the start of the next line
        if (*p < ' ')     *p = 0;                 //terminate the line at any invalid characters but keep going to find the start of the next line
        if (*p >= 0x7f)   *p = 0;                 //terminate the line at any invalid characters but keep going to find the start of the next line
        p++;
    }
}

static void splitRequest(char* p, char** ppMethod, char** ppPath, char** ppQuery)
{        
    *ppMethod   = NULL;
    *ppPath     = NULL;
    *ppQuery    = NULL;

    while (*p == ' ')         //Move past any leading spaces
    {
        if (*p == 0) return;
        p++;
    }
    *ppMethod = p;            //Record the start of the method (GET or POST)
 
    while (*p != ' ')         //Move past the method
    {
        if (*p == 0) return;
        p++;
    } 
    *p = 0;                   //Terminate the method
    p++;                      //Start at next character

    while (*p == ' ')         //Move past any spaces
    {
        if (*p == 0) return;
        p++;
    } 
    *ppPath = p;              //Record the start of the path
    
    while (*p != ' ')         //Move past the path and query
    {
        if (*p == 0) return;
        if (*p == '?')
        {
            *p = 0;           //Terminate the path
            *ppQuery = p + 1; //Record the start of the query
        }
        p++;
    }
    *p = 0;                   //Terminate the path or query
}
static void splitHeader(char* p, char** ppName, char** ppValue)
{
    *ppName    = p;                     //Record the start of the name
    *ppValue   = NULL;

    while (*p != ':')                   //Loop to an ':'
    {
        if (!*p) return;
        p++;
    }
    *p = 0;                             //Terminate the name by replacing the ':' with a NUL char
    p++;
    while (*p == ' ')                   //Move past any spaces
    {
        if (*p == 0) return;
        p++;
    }
    *ppValue = p;                       //Record the start of the value
}
int HttpRequestRead(char *pData, int len, char** ppMethod, char** ppPath, char** ppQuery, char** ppLastModified, char** ppCookies, int* pContentLength)
{
    char* pEnd = pData + len;
    char* pThis = pData;
    char* pNext = terminateThisAndGetNextLine(pThis, pEnd);
    splitRequest(pThis, ppMethod, ppPath, ppQuery);

    *ppLastModified = NULL; //Return NULL if no 'If-Modified-Since' line
    *ppCookies      = NULL; //Return NULL if no 'Cookie'            line
    *pContentLength = 0;    //Return 0    if no 'Content-Length'    line
    while(pNext)
    {
        pThis = pNext;
        pNext = terminateThisAndGetNextLine(pThis, pEnd);
        if (*pThis == 0) break;     //This line is empty ie no more headers
        char* pName;
        char* pValue;
        splitHeader(pThis, &pName, &pValue);
        if (HttpSameStrCaseInsensitive(pName, "If-Modified-Since")) *ppLastModified = pValue;
        if (HttpSameStrCaseInsensitive(pName, "Cookie"           )) *ppCookies      = pValue;
        if (HttpSameStrCaseInsensitive(pName, "Content-Length"   )) *pContentLength = atoi(pValue);
    }
    if (pNext) return pNext - pData;
    else       return len;
}
