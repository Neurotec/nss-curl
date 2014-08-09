#ifndef DOGROUP_H
#define DOGROUP_H


void
start_group_index ();

void
increment_group_index ();

int
get_group_index ();

/* Fetch remote group file with structure /etc/group.  */
enum nss_status 
do_group (char *content, char *name, struct group *result_buf, 
	 char *buf, size_t buflen, int *errnop, int *_);

#endif
