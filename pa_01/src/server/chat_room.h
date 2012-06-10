#ifndef _CHAT_ROOM_H_
#define _CHAT_ROOM_H_

#include "../common/list.h"

struct client;

typedef struct chat_room {
  char * name;
  list_t * clients;
} chat_room_t;

chat_room_t * chat_room_new(const char * name);
void chat_room_delete(void * room);
int chat_room_client_count(const chat_room_t * room);
void chat_room_add_client(chat_room_t * room, struct client * client);
void chat_room_remove_client(chat_room_t * room, struct client * client);
bool chat_room_has_client(const chat_room_t * room, const struct client * client);

#endif /* _CHAT_ROOM_H_ */
