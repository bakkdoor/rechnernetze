#ifndef _SERVER_CONNECTION_H_
#define _SERVER_CONNECTION_H_

#include "messages.h"

int connection_setup(int port);
int connection_close(int sockfd);
int connection_accept(int sockfd);
void connection_handle(int client_sock);

#endif /* _SERVER_CONNECTION_H_ */
