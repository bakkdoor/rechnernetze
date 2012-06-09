#ifndef _CLIENT_CONNECTION_H_
#define _CLIENT_CONNECTION_H_

#include <stdbool.h>

#include "../common/messages.h"

/**
 Client connection struct. Holds Server network information (socket, hostname,...) and username.
 */
typedef struct client_connection client_connection_t;

/**
 * Start connection (setup socket, start request to communicate as user with given name).
 * @param server_hostname server hostname or ip address
 * @param server_port server port
 * @param username user name
 * @return sonnection struct or NULL if can not communicate with server or username is allready in use
 */
client_connection_t * connection_setup(const char * server_hostname, const char * server_port, const char * username);

/**
 * Parse given buffer and creates a client message struct for this.
 * @return client message struct or NULL if fail (wrong input,...)
 */
client_message_t * parse_client_message(const char * buf);

/**
 * Handle input data (typed in from user at stdin or incomming from server).
 * @param cli_conn connection struct
 * @param timeout_sec timeout in seconds
 */
void connection_handle_socks(client_connection_t * cli_conn, int timeout_sec);


#endif /* _CLIENT_CONNECTION_H_ */
