#include <stdlib.h>
#include <arpa/inet.h>

#include "client.h"
#include "../common/chat_user.h"

struct client {
  chat_user_t * chat_user;
  struct sockaddr_in * addr;
  int sock;
};

client_t * client_new(chat_user_t * chat_user, struct sockaddr_in * client_addr) {
  client_t * client;

  client = calloc(1, sizeof(client_t));
  if (!client) {
    //TODO
  }

  client->addr->sin_family = AF_INET;
  client->addr->sin_port = 0;
  client->addr->sin_addr.s_addr = client_addr->sin_addr.s_addr;
  client->sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (client->sock < 0) {
    //TODO
  }

  if (bind(client->sock, (struct sockaddr *)client->addr, sizeof(struct sockaddr_in)) < 0) {
    // TODO
  }

  if (getsockname(client->sock, (struct sockaddr *)client->addr, sizeof(struct sockaddr_in)) < 0) {
    // TODO
  };

  return client;
}

// list function to delete client connection
void client_delete(void * _client) {
  client_t * client = _client;

  if(!client)
    return;

  chat_user_delete(client->chat_user);
  free(client->addr);
  free(client);
}
