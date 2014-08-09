#ifndef DOHOST_H
#define DOHOST_H

/* try locate IP from remote HOST, failed if:
   HTTP BODY must be <ip> <ip> <ip>..  */
enum nss_status 
do_gethostbyname (char *content, char *name, struct hostent *result_buf,
		  char *buf, size_t buflen, int *errnop, int *h_errnop);

#endif
