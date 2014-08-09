#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>

int url_from_config(const char *config_path, const char *key, char **value) {
  FILE *fp = NULL;

  fp = fopen(config_path, "r");
  if(fp == NULL) {
    syslog(LOG_ERR, "load_config(): %s", strerror(errno));
    return -1;
  }
  char *cmd;
  char url[BUFSIZ];
  int loaded = 0;
  char *url_ptr;

  while((fgets(url, BUFSIZ, fp)) != NULL){
    url_ptr = url;
    //syslog(LOG_NOTICE, "load_config(): readed line %s", url);
    cmd = strsep(&url_ptr, " ");
    if(cmd == NULL) { break; }
    //syslog(LOG_NOTICE, "load_config(): parsed command %s and url %s", cmd, url_ptr);
    if(strcmp(cmd, key) == 0) {
      *value = strdup(url_ptr);
      fclose(fp);
      return 0;
    }
  }

  fclose(fp);
  return -1;
}
