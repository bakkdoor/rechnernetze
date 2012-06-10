#ifndef _SERVER_CONNECTION_H_
#define _SERVER_CONNECTION_H_

#include <stdbool.h>
#include <netinet/in.h>

#include "../common/messages.h"
#include "chat_room.h"
#include "client.h"

typedef struct server_connection server_connection_t;

server_connection_t * server_connection_new(in_port_t port);
void server_connection_delete(server_connection_t * server_conn);
bool server_connection_close(server_connection_t * server_conn);
void server_connection_handle_incoming(server_connection_t * server_conn);
void server_connection_room_broadcast(server_connection_t * server_conn, server_message_t * msg, chat_room_t * room);
bool server_connection_has_client(server_connection_t * server_conn, client_t * client);
void server_connection_add_room(server_connection_t * server_conn, chat_room_t * room);
chat_room_t * server_connection_find_room(server_connection_t * server_conn, char * room_name);

#endif /* _SERVER_CONNECTION_H_ */
