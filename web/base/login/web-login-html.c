#include "web/http/http.h"
#include "web/web-add.h"
#include "web-login.h"
#include "web/base/web-pages-base.h"

void WebLoginHtml()
{
    HttpOk("text/html; charset=UTF-8", "no-cache", NULL, NULL);
    WebAddHeader("Login", NULL, NULL);
    HttpAddText(
"<style>"
#include "web-login-css.inc"
"</style>"
    );
    WebAddH1("Login");
    if (WebLoginPasswordIsSet())
    {
        WebAddH2("Welcome - please enter the password");
    }
    else
    {
        WebAddH2("Please enter a new password following user reset");
        HttpAddText("<p>Be careful to make it the one people expect!</p>");
    }
    
    HttpAddText("<form action='/login' method='get' autocomplete='off'>\r\n");
    HttpAddText("  <div style='width:8em; display:inline-block;'>Password</div>\r\n");
    HttpAddF   ("  <input type='hidden' name='todo'     value='%d'>\r\n", WebLoginOriginalToDo);
    HttpAddText("  <input type='text'   name='password' value='' autofocus>\r\n");
    HttpAddText("  <input type='submit'                 value='' >\r\n");
    HttpAddF   ("</form>\r\n");

    WebAddEnd();
}

/*
Device sends request for resource to server
Server cannot validate the session cookie (or there isn't one) so sends login form with original resource number as a hidden input
Device does a GET for /login with query containing password and original resource
Server validates password and sends original resource with a valid session cookie.
*/