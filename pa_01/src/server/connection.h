#ifndef _SERVER_CONNECTION_H_
#define _SERVER_CONNECTION_H_

#include "common/bool.h"
#include "messages.h"

typedef struct connection connection_t;
typedef struct server_connection server_connection_t;

server_connection_t * server_connection_new(int port);
bool server_connection_close(server_connection_t * conn);
connection_t * connection_accept(server_connection_t * server_conn);
void connection_handle(connection_t * client_conn);

#endif /* _SERVER_CONNECTION_H_ */
