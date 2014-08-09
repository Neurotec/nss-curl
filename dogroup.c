#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <nss.h>
#include <grp.h>

static int group_index = 0;

void
start_group_index ()
{
  group_index = 0;
}

void
increment_group_index ()
{
  group_index += 1;
}

int
get_group_index ()
{
  return group_index;
}

enum nss_status 
do_group (char *content, char *name, struct group *result_buf, 
	  char *buf, size_t buflen, int *errnop, int *_) 
{
  
  char *gr_name = strsep (&content, ":");
  char *gr_gid = strsep (&content, ":");
  char *gr_passwd = strsep (&content,":");
  
  syslog (LOG_NOTICE, "content [%s] name %s gid %d password %s", content, gr_name, atoi (gr_gid), gr_passwd);
  
  memset (buf, 0, buflen);
  result_buf->gr_name = strcpy (buf, gr_name);
  result_buf->gr_gid = atoi (gr_gid);
  result_buf->gr_passwd = strcpy (buf + strlen (gr_name) + 1, gr_passwd);
  
  char *start_mem = (buf + strlen (gr_name) + strlen(gr_passwd) + 2);
  char **members;
  char *member;
  size_t lengths= 0;
  int member_count = 0;
  size_t member_size = 0;
  size_t strings_offset = 0;
  //for count member
  char *copy_content = strdup (content);

  member = strsep (&content, ",");
  if(member == NULL) 
    {
      result_buf->gr_mem = (char **)(uintptr_t)start_mem;
    } 
  else 
    {
      //count members
      while(strsep (&copy_content, ",") != NULL) 
	{
	  member_size += 1;
	}
      free (copy_content);

      strings_offset = (member_size + 1) * sizeof(char *);
      members = (char**) start_mem ;
      members[0] = start_mem + strings_offset;
      strcpy (members[0], member);
      lengths = strlen (member);

      member_count += 1;
      while((member = strsep (&content, ",")) != NULL)
	{

	  members[member_count] = members[member_count - 1] + lengths + strlen (member) + 1;
	  strcpy (members[member_count], member);
	  member_count += 1;
	}

      members[member_count] = NULL;
      result_buf->gr_mem = (char **)(uintptr_t)start_mem;
    }
  free(copy_content);
  return NSS_STATUS_SUCCESS;
}
