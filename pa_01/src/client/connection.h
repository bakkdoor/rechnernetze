#ifndef _CLIENT_CONNECTION_H_
#define _CLIENT_CONNECTION_H_

#include <stdbool.h>

#include "../common/messages.h"

typedef struct client_connection client_connection_t;

client_connection_t * connection_setup(const char * server_hostname, const char * server_port, const char * username);
int connection_close(client_connection_t * cli_conn);
void connection_delete(client_connection_t * cli_conn);
int connection_send_client_message(client_connection_t * cli_conn, client_message_t * msg);
server_message_t * connection_recv_client_message(client_connection_t * cli_conn);
int connection_has_incoming_data(client_connection_t * cli_conn, int timeout_sec);

#endif /* _CLIENT_CONNECTION_H_ */
