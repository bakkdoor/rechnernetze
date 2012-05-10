#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "connection.h"

struct connection {
  int sock;
  struct sockaddr_in addr;
};

struct server_connection {
  int sock;
  int port;
  /* struct sockaddr_in addr; */
};

server_connection_t * server_connection_new(int port)
{
  int sockfd;

  server_connection_t * server_conn = calloc(1, sizeof(server_connection_t));
  if(!server_conn)
    return NULL;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0) {
    fprintf(stderr, "Could not setup server socket\n");
    exit(1);
  }

  server_conn->sock = sockfd;
  server_conn->port = port;

  return server_conn;
}

bool connection_close(server_connection_t * conn)
{
  if(!conn)
    return false;

  return close(conn->sock);
}

connection_t * connection_accept(server_connection_t * server_conn)
{
  /* TODO */
  return 0;
}

void connection_handle(connection_t * client_conn)
{
  /* TODO  */
}
