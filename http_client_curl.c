#include "http_client.h"
#include <stdio.h>
#include <string.h>

#include <curl/curl.h>
#include <syslog.h>

void 
http_client_init () 
{
  curl_global_init (CURL_GLOBAL_ALL);
}

void 
http_client_quit () {
  curl_global_cleanup ();
}

size_t 
write_data (void *ptr, size_t size, size_t nmemb, void *content)
{
  char **response_ptr = (char**)content;
  *response_ptr = strndup (ptr, (size_t)(size * nmemb));
  return size * nmemb;
}

int 
http_client_get (const char *url, char **content, long *http_code)
{
   CURL *curl = curl_easy_init ();
   curl_easy_setopt (curl, CURLOPT_URL, url);
   curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_data);

   curl_easy_setopt (curl, CURLOPT_WRITEDATA, content);
   CURLcode res = curl_easy_perform (curl);
   if(res != CURLE_OK) 
     {
       syslog (LOG_ERR, "curl_easy_perform(): failed: %s", curl_easy_strerror (res));
       curl_easy_cleanup (curl);
       return -1;
     }
   curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, http_code);
   curl_easy_cleanup (curl);
   return 0;
}
