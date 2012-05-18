#ifndef _CHAT_ROOM_H_
#define _CHAT_ROOM_H_

#include "list.h"
#include "chat_user.h"

struct chat_room;
typedef struct chat_room chat_room_t;

chat_room_t * chat_room_new(const char * name);
void chat_room_delete(void * room);
int chat_room_user_count(const chat_room_t * room);
void chat_room_add_user(chat_room_t * room, chat_user_t * user);
void chat_room_send_message(const chat_user_t * user, const char * message);

#endif /* _CHAT_ROOM_H_ */
