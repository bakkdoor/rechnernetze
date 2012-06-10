#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>

#include "client.h"
#include "chat_room.h"
#include "../common/output.h"

client_t * client_new(char * name) {
  client_t * client;
  unsigned int slen = sizeof(struct sockaddr_in);
  struct sockaddr_in addr;

  client = calloc(1, sizeof(client_t));
  if (!client) {
    error(false, "Could not create new client");
    return NULL;
  }
  client->addr = calloc(1, sizeof(struct sockaddr_in));
  if(!client->addr) {
    error(false, "Could not create addr for new client");
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(0);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  client->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client->sock < 0) {
    error(false, "Could not setup user-specific socket for user %s", name);
    free(client);
    return NULL;
  }

  if (bind(client->sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
    error(false, "Could not bind on user-specific port for user %s", name);
    free(client);
    return NULL;
  }

  if (getsockname(client->sock, (struct sockaddr *)&addr, &slen) < 0) {
    perror("getsockname()");
    error(false, "getsockname() failed");
    free(client);
    return NULL;
  } else {
    client->port = ntohs(addr.sin_port);
    info("User-specific port %d opened for user %s", client->port, name);
  }

  fcntl(client->sock, F_SETFL, O_NONBLOCK);

  client->name = calloc(strlen(name) + 1, sizeof(char));
  strcpy(client->name, name);

  client->rooms = list_new();

  return client;
}

// list function to delete client connection
void client_delete(void * _client) {
  client_t * client = _client;

  assert(client);

  free(client->name);
  list_delete(client->rooms, NULL);
  free(client->addr);
  free(client);
}

int client_send_message(const client_t * client, server_message_t * message)
{
  char buf[MAX_SERVER_MSG_SIZE];
  size_t len = server_message_write(message, buf);

  return sendto(client->sock, buf, len, 0,
                (struct sockaddr *) client->addr,
                sizeof(struct sockaddr_in));
}

client_message_t * client_read_message(const client_t * client)
{
  char buf[MAX_CLIENT_MSG_SIZE];
  unsigned int bytes_read;
  unsigned int slen = sizeof(struct sockaddr_in);
  memset(buf, 0, MAX_CLIENT_MSG_SIZE);

  bytes_read = recvfrom(client->sock, buf, MAX_CLIENT_MSG_SIZE, 0,
                        (struct sockaddr *) client->addr,
                        (socklen_t *) &slen);

  if(bytes_read < 1) {
    return NULL;
  }

  return client_message_read(buf);
}

void client_join_room(client_t * client, chat_room_t * room)
{
  assert(client && room);

  list_insert(client->rooms, room);
  chat_room_add_client(room, client);
}

void client_leave_room(client_t * client, chat_room_t * room)
{
  assert(client && room);

  chat_room_remove_client(room, client);
  list_remove(client->rooms, room, true, NULL, NULL);
}
