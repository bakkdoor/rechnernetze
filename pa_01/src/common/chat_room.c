#include <stdlib.h>
#include <string.h>
#include "chat_room.h"
#include "list.h"

chat_room_t * chat_room_new(const char * name)
{
  chat_room_t * room = calloc(1, sizeof(chat_room_t));
  if(!room)
    return NULL;

  room->name = calloc(strlen(name) + 1, sizeof(char));
  room->users = list_new();

  return room;
}

void chat_room_delete(void * _room)
{
  chat_room_t * room = _room;
  if(!room)
    return;

  if(list_size(room->users) == 0) {
    list_delete(room->users, NULL);
    free(room->name);
    free(room);
  }
}

int chat_room_user_count(const chat_room_t * room)
{
  if(!room)
    return 0;

  return list_size(room->users);
}

void chat_room_add_user(chat_room_t * room, chat_user_t * user)
{
  if(!room)
    return;
  if(!user)
    return;

  list_insert(room->users, user);
}

void chat_room_send_message(const chat_user_t * user, const char * message)
{
  /* TODO */
}
