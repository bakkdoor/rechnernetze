#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "common/error.h"
#include "connection.h"

struct server_connection {
  int sock;
  int port;
  struct sockaddr_in * addr;
};

server_connection_t * server_connection_new(int port)
{
  int sockfd, err;
  struct sockaddr_in * addr;

  server_connection_t * server_conn = calloc(1, sizeof(server_connection_t));
  if(!server_conn)
    return NULL;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0) {
    error("Could not setup server socket\n", true);
  }

  server_conn->sock = sockfd;
  server_conn->port = port;

  addr = calloc(1, sizeof(struct sockaddr_in));
  if(!addr) {
    error("Could not setup sockaddr_in struct", true);
  }

  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  addr->sin_addr.s_addr = htonl(INADDR_ANY);

  err = bind(sockfd, (struct sockaddr *) addr, sizeof(struct sockaddr_in));
  if(err < 0) {
    error("Could not bind on port\n", true);
  }

  server_conn->addr = addr;

  return server_conn;
}

void server_connection_delete(server_connection_t * server_conn)
{
  if(!server_conn)
    return;

  free(server_conn->addr);
  free(server_conn);
}

bool connection_close(server_connection_t * server_conn)
{
  if(!server_conn)
    return false;

  return close(server_conn->sock);
}

void server_connection_handle_incoming(server_connection_t * server_conn)
{
  /* TODO:
     - recvfrom(server_conn->sock);
     - dispatch messages to connected clients on same chat_room
     - ?
   */
}
