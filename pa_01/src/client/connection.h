#ifndef _CLIENT_CONNECTION_H_
#define _CLIENT_CONNECTION_H_

#include "messages.h"
#include "../server/messages.h"

typedef struct client_connection client_connection_t;

client_connection_t * connection_setup(const char * server_hostname, int server_port, char * username);
int connection_close(int sockfd);
int connection_send_client_message(client_connection_t * cli_conn, client_message_t * msg);
server_message_t * connection_recv_client_message(client_connection_t * cli_conn);

#endif /* _CLIENT_CONNECTION_H_ */
