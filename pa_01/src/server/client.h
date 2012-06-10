#ifndef _SERVER_CLIENT_H
#define _SERVER_CLIENT_H

#include <arpa/inet.h>

#include "chat_room.h"
#include "../common/messages.h"

typedef struct client {
  char * name;
  list_t * rooms;

  struct sockaddr_in * addr;
  int sock;
  in_port_t port;
} client_t;

client_t * client_new(char * name, struct sockaddr_in * client_addr);
void client_delete(void * client);
int client_send_message(const client_t * client, server_message_t * message);
client_message_t * client_read_message(const client_t * client);
void client_join_room(client_t * client, chat_room_t * room);
void client_leave_room(client_t * client, chat_room_t * room);

#endif /* _SERVER_CLIENT_H */
