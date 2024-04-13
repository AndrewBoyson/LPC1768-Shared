//#include "web-this/web-site-name.h"
#include "web/web.h"

#define SESSION_LIFE 10 * 365 * 24 * 60 * 60

//Do not use ' ', ';', '=' as they are part of the cookie protocol: name1=value1; name2=value2; name3=value3 etc
static char sessionName[9]; //Limit the name to 8 characters (plus terminating zero) to keep things quick

char* WebLoginSessionNameGet()
{
    return sessionName;
}

void WebLoginSessionNameCreate()
{
    //Make the session name from the site name - eg  "Heating" and "GPS Clock" will be "heat_sid" and "gpsc_sid"
    int i = 0;
    int j = 0;
    while (1)
    {
        if (i >= sizeof(sessionName) - 4 - 1) break; //Leave room for the "_sid" and the terminating NUL character
        //char c = WEB_SITE_NAME[j++];
		if (!WebSiteName) break;                     //Stop if web site name has not been set
        char c = WebSiteName[j++];
        if (!c) break;                               //Stop if run out of site name
        if (c >= 'A' && c <= 'Z') c |= 0x20;         //Make lower case
        if (c < 'a' || c > 'z') continue;            //Skip anything other than letters
        sessionName[i++] = c;                        //Add the first characters of the site name for which there is room
    }
    for (int k = 0; k < 4; k++)                      //Add "_sid"
    {
        sessionName[i++] = "_sid"[k];
    }
    sessionName[i] = 0;                              //Add the terminating NUL character
}

int WebLoginSessionNameLife()
{
    return SESSION_LIFE;
}
