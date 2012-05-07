#ifndef _CHAT_USER_H_
#define _CHAT_USER_H_

#include "list.h"

typedef struct chat_user {
  char * name;
  list_t * rooms;
  int sockfd;
} chat_user_t;

chat_user_t * chat_user_new(char * name, int sockfd);
void chat_user_delete(void * _user);

#endif /* _CHAT_USER_H_ */
