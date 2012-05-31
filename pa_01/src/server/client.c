#include <stdlib.h>
#include <arpa/inet.h>

#include "client.h"
#include "../common/chat_user.h"

client_t * client_new(chat_user_t * chat_user, struct sockaddr_in * client_addr, int port) {
  client_t * client;

  client = calloc(1, sizeof(client_t));
  if (!client) {
    //TODO
  }
  client->addr = calloc(1, sizeof(struct sockaddr_in));

  client->addr->sin_family = AF_INET;
  client->addr->sin_port = port;
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

void client_send_message(client_t * client, server_message_t * message)
{
  /* TODO: write into buf, then send buffer over via client->sock */
}

client_message_t * client_read_message(const client_t * client)
{
  char * buf;
  int bytes_read;
  client_message_t * incoming_message = NULL;

  buf = calloc(MAX_CLIENT_MSG_SIZE, sizeof(char));

  bytes_read = recvfrom(client->sock, buf, sizeof(buf), 0,
                        (struct sockaddr *) client->addr,
                        (socklen_t *) sizeof(struct sockaddr_in));

  if(bytes_read < 1) {
    free(buf);
    return NULL;
  }

  incoming_message = client_message_read(buf);
  free(buf);
  return incoming_message;
}
