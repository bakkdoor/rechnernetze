#ifndef _SERVER_CONNECTION_H_
#define _SERVER_CONNECTION_H_

#include "messages.h"

int connection_setup(int port);
int connection_close(int sockfd);

#endif /* _SERVER_CONNECTION_H_ */
