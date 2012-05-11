#ifndef _CHAT_USER_H_
#define _CHAT_USER_H_

#include "list.h"

typedef struct chat_user chat_user_t;

chat_user_t * chat_user_new(const char * name);
void chat_user_delete(void * user);
list_t * chat_user_rooms(chat_user_t * user);
char * chat_user_name(chat_user_t * user);

#endif /* _CHAT_USER_H_ */
