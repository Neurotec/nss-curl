#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <nss.h>
#include <netinet/in.h>
#include <syslog.h>
#include <arpa/nameser.h>


enum nss_status 
do_gethostbyname (char *content, char *name, struct hostent *result_buf,
		  char *buf, size_t buflen, int *errnop, int *h_errnop) 
{
  size_t host_len = 0;
  char *host = NULL;
  char *addr_next = NULL;
  in_addr_t host_addr;
  char **h_addrs = NULL;

  if(content == NULL) 
    {
      h_errnop = NO_RECOVERY;
      return NSS_STATUS_UNAVAIL;
    }

  result_buf->h_name = name;
  result_buf->h_addrtype = AF_INET;
  result_buf->h_length = INADDRSZ;

  char *start_mem = buf + sizeof (struct hostent) + 1;
  h_addrs = (char **) buf;
  h_addrs[0] = start_mem;

  while((host = strsep (&content, " ")) != NULL)
    {
      
      if(!inet_aton (host, &host_addr))
	{
	  syslog(LOG_ERR, "do_gethostbyname(): failed inet_aton on %s", host);
	  continue;
	}

      memcpy(h_addrs[host_len], &host_addr, INADDRSZ);

      host_len += 1;
      h_addrs[host_len] = h_addrs[host_len-1] + INADDRSZ;
    }

  h_addrs[host_len] = NULL;
  result_buf->h_addr_list = (char **)(uintptr_t)buf;

  return NSS_STATUS_SUCCESS;
}
