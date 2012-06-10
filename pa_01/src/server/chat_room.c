#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "chat_room.h"
#include "client.h"

chat_room_t * chat_room_new(const char * name)
{
  chat_room_t * room = calloc(1, sizeof(chat_room_t));
  if(!room)
    return NULL;

  room->name = calloc(strlen(name) + 1, sizeof(char));
  strcpy(room->name, name);
  room->clients = list_new();

  return room;
}

void chat_room_delete(void * _room)
{
  chat_room_t * room = _room;
  assert(room);

  if(!room)
    return;

  if(list_size(room->clients) == 0) {
    list_delete(room->clients, NULL);
    free(room->name);
    free(room);
  }
}

int chat_room_user_count(const chat_room_t * room)
{
  assert(room);

  if(!room)
    return 0;

  return list_size(room->clients);
}

void chat_room_add_client(chat_room_t * room, client_t * client)
{
  assert(room && client);

  if(!room || !client)
    return;

  list_insert(room->clients, client);
}

void chat_room_remove_client(chat_room_t * room, client_t * client)
{
  assert(room && client);

  if (!room || !client)
    return;

  list_remove(room->clients, client, true, NULL, NULL);
}

static char * _username;
bool _chat_room_client_with_correct_name(const void * _client)
{
  const client_t * client = _client;
  assert(client);
  return strcmp(_username, client->name) == 0;
}

bool chat_room_has_client(const chat_room_t * room, const client_t * client)
{
  assert(room && client);
  if(!room || !client)
    return false;

  _username = client->name;

  if(list_find_first(room->clients, _chat_room_client_with_correct_name))
    return true;
  return false;
}
