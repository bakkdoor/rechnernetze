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

/**
 * Delete connection stuct. Release allocated memory.
 * @param cli_conn connection struct
 */
void connection_delete(client_connection_t * cli_conn);

/**
 * Handle given server message.
 * @param msg server message struct
 */
void handle_server_message(server_message_t * msg);

/**
 * Chack if connection socket has data to recieve.
 * @param cli_conn connection struct
 * @param timeout_sec timeout in seconds
 * @return > 0 if socket has incomming data, 0 else
 */
int connection_has_incoming_data(client_connection_t * cli_conn, int timeout_sec);

/**
 * Send message to server.
 * @param cli_conn connection struct
 * @param msg message to send
 * @return bytes send or -1 if fail
 */
int connection_send_client_message(client_connection_t * cli_conn, client_message_t * msg);

/**
 * Recieve and parse message from server.
 * @param cli_conn connection struct
 * @return parsed server message struct
 */
server_message_t * connection_recv_client_message(client_connection_t * cli_conn);

/**
 * Close connection. Send disconnect request to server and wait of reply. Close connection after server reply or three requests are send.
 * @param cli_conn connection struct
 */
void connection_close(client_connection_t * cli_conn);

#endif /* _CLIENT_CONNECTION_H_ */
