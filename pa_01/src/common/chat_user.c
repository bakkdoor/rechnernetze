#include <stdlib.h>
#include <string.h>
#include "chat_user.h"
#include "chat_room.h"

chat_user_t * chat_user_new(const char * name)
{
  chat_user_t * user = calloc(1, sizeof(chat_user_t));
  if(!user)
    return NULL;

  user->rooms = list_new();
  user->name = calloc(strlen(name) + 1, sizeof(char));
  strcpy(user->name, name);

  return user;
}

void chat_user_delete(void * _user)
{
  chat_user_t * user = _user;

  if(!user)
    return;

  free(user->name);
  list_delete(user->rooms, chat_room_delete);
  free(user);
}

list_t * chat_user_rooms(const chat_user_t * user)
{
  if(!user)
    return NULL;

  return user->rooms;
}

char * chat_user_name(const chat_user_t * user)
{
  if(!user)
    return NULL;

  return user->name;
}

static char * _room_name = "";
bool room_with_correct_name(const void * _room)
{
  const chat_room_t * room = _room;
  return strcmp(room->name, _room_name) == 0;
}

bool chat_user_in_room(const chat_user_t * user, const char * room_name)
{
  _room_name = (char*)room_name;
  if(list_find_first(user->rooms, room_with_correct_name))
    return true;
  return false;
}
