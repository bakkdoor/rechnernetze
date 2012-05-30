#ifndef _CHAT_USER_H_
#define _CHAT_USER_H_

#include "list.h"

typedef struct chat_user {
  char * name;
  list_t * rooms;
} chat_user_t;

chat_user_t * chat_user_new(const char * name);
void chat_user_delete(void * user);
list_t * chat_user_rooms(const chat_user_t * user);
char * chat_user_name(const chat_user_t * user);
bool chat_user_in_room(const chat_user_t * user, const char * room_name);

#endif /* _CHAT_USER_H_ */
