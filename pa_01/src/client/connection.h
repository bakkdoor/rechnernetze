#ifndef _CLIENT_CONNECTION_H_
#define _CLIENT_CONNECTION_H_

#include "messages.h"

int connection_setup(int port);
int connection_close(int sockfd);
int connection_send_client_message(int sock, client_message_t * msg);

#endif /* _CLIENT_CONNECTION_H_ */
