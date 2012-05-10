#ifndef _SERVER_CONNECTION_H_
#define _SERVER_CONNECTION_H_

#include "common/bool.h"
#include "messages.h"

typedef struct server_connection server_connection_t;

server_connection_t * server_connection_new(int port);
bool server_connection_close(server_connection_t * server_conn);
void server_connection_handle_incoming(server_connection_t * server_conn);

#endif /* _SERVER_CONNECTION_H_ */
