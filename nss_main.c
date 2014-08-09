//nss-curl - module nsswitch for remotes structs
#define _GNU_SOURCE
#include <nss.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <grp.h>
#include <errno.h>

#include "http_client.h"
#include "config.h"

#include "dohost.h"
#include "dogroup.h"

const char *program_name = "nss_curl";

const char *config_path = "/etc/nss-curl.conf";

/* Hi how are you?
   i fine thans.  */

/**
 * * gethostbyname http://localhost/gethostbyname?name=%s
 * * getgrent http://localhost/getgrent?index=%d
 */
char *url_gethostbyname = NULL;
char *url_getgrent = NULL;
char *url_getgrnam = NULL;

bool http_client_init_lock = false;

 
void 
_nss_curl_quit (void) {
  if(http_client_init_lock == false)
    return;
  
  http_client_quit ();

  syslog (LOG_NOTICE, "shutdown");

  closelog ();

  if(url_gethostbyname != NULL)
    {
      free (url_gethostbyname);
      url_gethostbyname = NULL;
    }

  if(url_getgrent != NULL)
    {
      free (url_getgrent);
      url_getgrent = NULL;
    }
  http_client_init_lock = false;
}


enum nss_status 
_nss_curl_init (void) 
{
  if(http_client_init_lock) 
    return NSS_STATUS_SUCCESS;

  http_client_init ();

  if(url_gethostbyname == NULL 
     && url_from_config (config_path, "gethostbyname", &url_gethostbyname) < 0) 
    {
      syslog (LOG_ERR, "can't find key *gethostbyname* on config file at %s", config_path);
      return NSS_STATUS_NOTFOUND;
    }
  
  if(url_getgrent == NULL 
     && url_from_config (config_path, "getgrent", &url_getgrent) < 0) 
    {
      syslog (LOG_ERR, "can't find key *getgrent* on config file at %s", config_path);
      return NSS_STATUS_NOTFOUND;
    }
  
  if(url_getgrnam == NULL 
     && url_from_config (config_path, "getgrnam", &url_getgrnam) < 0)
    {
      syslog (LOG_ERR, "can't find key *getgrnam* on config file at %s", config_path);
      return NSS_STATUS_NOTFOUND;
    }
  
  openlog (program_name, 0, 0);
  
  
  atexit (_nss_curl_quit);
  return NSS_STATUS_SUCCESS;
}


enum nss_status 
_nss_curl_gethostbyname_r (const char *name, struct hostent *result_buf,
			   char *buf, size_t buflen, int *errnop, int *h_errnop) 
{
  enum nss_status ret;
  char *url = NULL;

  _nss_curl_init ();
  url = malloc (strlen (url_gethostbyname) + strlen (name) + 1);
  sprintf (url, url_gethostbyname, name);
  ret = do_client (url, name, result_buf, buf, buflen, errnop, h_errnop, do_gethostbyname);
  free (url);

  return ret;
}


/**
 *#### GROUPS
 */

enum nss_status 
_nss_curl_setgrent ()
{
  start_group_index ();

  if(_nss_curl_init () != NSS_STATUS_SUCCESS) 
    {
      return NSS_STATUS_UNAVAIL;
    }
  
  return NSS_STATUS_SUCCESS;
}

enum nss_status 
_nss_curl_endgrent () 
{
  syslog (LOG_NOTICE, "endgrent");
  _nss_curl_quit ();
  return NSS_STATUS_SUCCESS;
}


enum nss_status 
_nss_curl_getgrent_r (struct group *result_buf, char *buf, size_t buflen, int *errnop)
{
  //content html
  void *_;
  char *http_content = NULL;
  long http_code;
  enum nss_status ret;
  syslog (LOG_NOTICE, "getgrent at %s", url_getgrent);
  
  char *url = malloc (sizeof(char) * strlen (url_getgrent) + 16 + 1);
  if(url == NULL)
    {
      syslog(LOG_ERR, "failed allocating memory");
      return NSS_STATUS_NOTFOUND;
    }
  sprintf (url, url_getgrent, get_group_index ());
  ret =  do_client (url, _, result_buf, buf, buflen, errnop, _, do_group);
  increment_group_index ();

  return ret;
}

enum nss_status 
_nss_curl_getgrnam_r (const char *name, struct group *result_buf,
		      char *buf, size_t buflen, int *errnop, int *_) 
{
  
  _nss_curl_init ();
  char *url = malloc (sizeof(char) * strlen (url_getgrnam) + strlen (name) + 1);
  if(url == NULL) 
    {
      syslog(LOG_ERR, "failed allocating memory");
      return NSS_STATUS_NOTFOUND;
    }
  sprintf (url, url_getgrnam, name);
  enum nss_status ret =  do_client (url, (void *)name, result_buf, buf, buflen, errnop, _, do_group);
  free (url);
  
  return NSS_STATUS_SUCCESS;
}



enum nss_status 
do_client (const char *url, void *user_data, 
	   void *result_buf, char *buf, size_t buflen, int *errnop, int *h_errnop,
	   enum nss_status (*load_func) (char *, void *, void *, char *, size_t, int*, int*)) 
{
  enum nss_status ret;
  char *http_content = NULL;
  long http_code;


  _nss_curl_init ();
  
  syslog (LOG_NOTICE, "do_client at %s", url);
  
  if(http_client_get (url, &http_content, &http_code) < 0) 
    {
      syslog(LOG_ERR, "failed http_client_get");
      return NSS_STATUS_UNAVAIL;
    }

  if(http_code != 200) 
    {
      syslog(LOG_NOTICE, "done != 200 ok");
      free(http_content);
      return NSS_STATUS_NOTFOUND;
    }

  syslog (LOG_NOTICE, "received ok at <%s>", url, http_content);
  
  ret = load_func (http_content, user_data, result_buf, buf, buflen, errnop, h_errnop);

  if(http_content)
    free (http_content);

  return ret;
}
