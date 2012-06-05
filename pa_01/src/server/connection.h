#ifndef _SERVER_CONNECTION_H_
#define _SERVER_CONNECTION_H_

#include "../common/messages.h"

typedef struct server_connection server_connection_t;

server_connection_t * server_connection_new(in_port_t port);
void server_connection_delete(server_connection_t * server_conn);
bool server_connection_close(server_connection_t * server_conn);
void server_connection_handle_incoming(server_connection_t * server_conn);
void server_connection_room_broadcast(server_connection_t * server_conn, server_message_t * msg, char * room);

#endif /* _SERVER_CONNECTION_H_ */
