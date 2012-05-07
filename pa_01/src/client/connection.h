#ifndef _CLIENT_CONNECTION_H_
#define _CLIENT_CONNECTION_H_

#include "messages.h"

int connection_setup(int port);
int connection_close(int sockfd);

#endif /* _CLIENT_CONNECTION_H_ */
