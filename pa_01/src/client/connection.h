#ifndef _CLIENT_CONNECTION_H_
#define _CLIENT_CONNECTION_H_

#include <stdbool.h>

#include "../common/messages.h"

typedef struct client_connection client_connection_t;

client_connection_t * connection_setup(const char * server_hostname, const char * server_port, const char * username);
void connection_close(client_connection_t * cli_conn);
void connection_delete(client_connection_t * cli_conn);
int connection_send_client_message(client_connection_t * cli_conn, client_message_t * msg);
server_message_t * connection_recv_client_message(client_connection_t * cli_conn);
int connection_has_incoming_data(client_connection_t * cli_conn, int timeout_sec);

client_message_t * parse_client_message(const char * buf);
void connection_handle_socks(client_connection_t * cli_conn, int timeout_sec);
void handle_server_message(server_message_t * msg);


#endif /* _CLIENT_CONNECTION_H_ */
