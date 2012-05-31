#ifndef _SERVER_CLIENT_H
#define _SERVER_CLIENT_H

#include <arpa/inet.h>

#include "../common/chat_user.h"
#include "../common/messages.h"

typedef struct client {
  chat_user_t * chat_user;
  struct sockaddr_in * addr;
  int sock;
} client_t;

client_t * client_new(chat_user_t * char_user, struct sockaddr_in * client_addr, int port);
void client_delete(void * client);
int client_send_message(client_t * client, server_message_t * message);
client_message_t * client_read_message(const client_t * client);

#endif /* _SERVER_CLIENT_H */
