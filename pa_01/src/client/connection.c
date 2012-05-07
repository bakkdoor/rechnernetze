#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "connection.h"

int connection_setup(int port)
{
  /* TODO */
  /* int sockfd; */
  return 0;
}

int connection_close(int sockfd)
{
  return close(sockfd);
}
