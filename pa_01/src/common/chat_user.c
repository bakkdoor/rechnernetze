#include <stdlib.h>
#include <string.h>
#include "chat_user.h"
#include "chat_room.h"

struct chat_user {
  char * name;
  list_t * rooms;
};

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

list_t * chat_user_rooms(chat_user_t * user)
{
  if(!user)
    return NULL;

  return user->rooms;
}

char * chat_user_name(chat_user_t * user)
{
  if(!user)
    return NULL;

  return user->name;
}
