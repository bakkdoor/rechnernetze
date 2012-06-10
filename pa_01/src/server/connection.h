#ifndef _SERVER_CONNECTION_H_
#define _SERVER_CONNECTION_H_

#include <stdbool.h>
#include <netinet/in.h>

#include "../common/messages.h"
#include "chat_room.h"
#include "client.h"

typedef struct server_connection server_connection_t;

/**
 * @param port Port to listen on.
 * @return New Server connection.
 **/
server_connection_t * server_connection_new(in_port_t port);

/**
 * @param server_conn Server connection to be deleted.
 **/
void server_connection_delete(server_connection_t * server_conn);

/**
 * @param server_conn Server connection to be closed.
 **/
bool server_connection_close(server_connection_t * server_conn);

/**
 * @param server_conn Server connection to handle incoming messages for.
 **/
void server_connection_handle_incoming(server_connection_t * server_conn);

/**
 * @param server_conn Server connection to broadcast to a room for.
 * @param msg Server message to broadcast.
 * @param room Chat room to broadcast msg to.
 **/
void server_connection_room_broadcast(server_connection_t * server_conn, server_message_t * msg, chat_room_t * room);

/**
 * @param server_conn Server connection to check if client exists.
 * @param client Client to check for.
 * @return true if server_conn has client, false otherwise.
 **/
bool server_connection_has_client(server_connection_t * server_conn, client_t * client);

/**
 * @param server_conn Server connection to add a room to.
 * @param room Room to be added to server_conn.
 **/
void server_connection_add_room(server_connection_t * server_conn, chat_room_t * room);

/**
 * @param server_conn Server connection to find a room in.
 * @param room_name Name of room to be found.
 * @return Chat room (if found) or NULL.
 **/
chat_room_t * server_connection_find_room(server_connection_t * server_conn, char * room_name);

#endif /* _SERVER_CONNECTION_H_ */
