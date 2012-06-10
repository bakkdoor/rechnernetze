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

/**
 * @param name Username of client
 * @param client_addr sock_addr_in for client
 * @return New client struct.
 **/
client_t * client_new(char * name, struct sockaddr_in * client_addr);

/**
 * @param client Client to be deleted.
 * Deletes a client.
 **/
void client_delete(void * client);

/**
 * @param client Client to send server message message to.
 * @message Server message to send to client.
 * @return Amount of bytes sent.
 **/
int client_send_message(const client_t * client, server_message_t * message);

/**
 * @param client Client to read incoming client message from.
 * @return Client message read from client.
 **/
client_message_t * client_read_message(const client_t * client);

/**
 * @param client Client to join a room.
 * @param room Room to join.
 **/
void client_join_room(client_t * client, chat_room_t * room);

/**
 * @param client Client to leave room.
 * @param room Room to leave.
 **/
void client_leave_room(client_t * client, chat_room_t * room);

#endif /* _SERVER_CLIENT_H */
