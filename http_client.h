#ifndef _HTTP_CLIENT_
#define _HTTP_CLIENT_

#include <stdio.h>

void http_client_init();

void http_client_quit();

int http_client_get(const char *url, char **content, long *http_code);

#endif
