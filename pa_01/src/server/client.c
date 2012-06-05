#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "client.h"
#include "../common/chat_user.h"
#include "../common/output.h"

client_t * client_new(chat_user_t * chat_user, struct sockaddr_in * client_addr) {
  client_t * client;
  int slen = sizeof(struct sockaddr_in);

  client = calloc(1, sizeof(client_t));
  if (!client) {
    error(false, "Could not create new client");
    return NULL;
  }
  client->addr = calloc(1, sizeof(struct sockaddr_in));
  if(!client->addr) {
    error(false, "Could not create addr for new client");
  }

  client->addr->sin_family = AF_INET;
  client->addr->sin_port = htons(0);
  client->addr->sin_addr.s_addr = client_addr->sin_addr.s_addr;
  client->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client->sock < 0) {
    error(false, "Could not setup user-specific socket for user %s", chat_user->name);
  }

  if (bind(client->sock, (struct sockaddr *)client->addr, sizeof(struct sockaddr_in)) < 0) {
    error(false, "Could not bind on user-specific port for user %s", chat_user->name);
    return NULL;
  } else {
    client->port = ntohs(client->addr->sin_port);
    info("User-specific port %d opened for user %s", client->port, chat_user->name);
  }

  if (getsockname(client->sock, (struct sockaddr *)client->addr, &slen) < 0) {
    perror("getsockname()");
    error(false, "getsockname() failed");
  }

  fcntl(client->sock, F_SETFL, O_NONBLOCK);

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

int client_send_message(const client_t * client, server_message_t * message)
{
  int bytes_sent;
  char * buf = calloc(MAX_SERVER_MSG_SIZE, sizeof(char));
  size_t len = server_message_write(message, buf);

  bytes_sent = sendto(client->sock, buf, len, 0,
                      (struct sockaddr *) client->addr,
                      sizeof(struct sockaddr_in));

  free(buf); /* ?!?! TODO: Check if OK here. */
  return bytes_sent;
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
