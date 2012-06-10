#ifndef _CHAT_ROOM_H_
#define _CHAT_ROOM_H_

#include "../common/list.h"

struct client;

typedef struct chat_room {
  char * name;
  list_t * clients;
} chat_room_t;

/**
 * @param name Name of new chat room.
 * @return New chat room or NULL on failure.
 **/
chat_room_t * chat_room_new(const char * name);

/**
 * @param room chat_room to be deleted.
 **/
void chat_room_delete(void * room);

/**
 * @param room chat_room to get client count for.
 * @return Number of clients in room.
 **/
int chat_room_client_count(const chat_room_t * room);

/**
 * @param room Chat room to add client to.
 * @param client Client to be added to room.
 **/
void chat_room_add_client(chat_room_t * room, struct client * client);

/**
 * @param room Chat room to remove client from.
 * @param client Client to be removed from room.
 **/
void chat_room_remove_client(chat_room_t * room, struct client * client);

/**
 * @param room Chat room to check client for.
 * @param client Client to be checked in room.
 * @return true if chat room has client, false otherwise.
 **/
bool chat_room_has_client(const chat_room_t * room, const struct client * client);

#endif /* _CHAT_ROOM_H_ */
