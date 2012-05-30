#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>


#include "connection.h"
#include "client.h"
#include "../common/list.h"
#include "../common/chat_room.h"g
#include "../common/output.h"
#include "../client/messages.h"

struct server_connection {
  int sock;
  int port;
  struct sockaddr_in * addr;
  list_t  * rooms;
  list_t  * clients;
};

////////////////////////////////////////////////////////

server_connection_t * server_connection_new(int port)
{
  int sockfd, err;
  struct sockaddr_in * addr;

  server_connection_t * server_conn = calloc(1, sizeof(server_connection_t));
  if(!server_conn)
    return NULL;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0) {
    error(true, "Could not setup server socket");
  }

  server_conn->sock = sockfd;
  server_conn->port = port;

  addr = calloc(1, sizeof(struct sockaddr_in));
  if(!addr) {
    error(true, "Could not setup sockaddr_in struct");
  }

  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  addr->sin_addr.s_addr = htonl(INADDR_ANY);

  err = bind(sockfd, (struct sockaddr *) addr, sizeof(struct sockaddr_in));
  if(err < 0) {
    error(true, "Could not bind on port");
  }

  server_conn->addr = addr;

  server_conn->rooms = list_new();
  server_conn->clients = list_new();

  return server_conn;
}

void server_connection_delete(server_connection_t * server_conn)
{
  if(!server_conn)
    return;

  list_delete(server_conn->rooms, chat_room_delete);
  list_delete(server_conn->clients, client_delete);
  free(server_conn->addr);
  free(server_conn);
}

bool connection_close(server_connection_t * server_conn)
{
  if(!server_conn)
    return false;

  list_foreach(server_conn->clients, client_delete);

  return close(server_conn->sock);
}

void server_connection_handle_incoming(server_connection_t * server_conn)
{
  /* TODO:
     - recvfrom(server_conn->sock);
     - dispatch messages to connected clients on same chat_room
     - ?
   */

  /* int bytes_read; */
  /* char * buf, * tmp; */
  /* size_t len; */
  /* char type; */
  /* client_message_t * incoming_message = NULL; */
  /* struct sockaddr_in * incoming_addr; */
  /* client_t * client; */

  /* buf = calloc(MAX_CLIENT_MSG_SIZE, sizeof(char)); */

  /* bytes_read = recvfrom(server_conn->sock, buf, sizeof(buf), 0, */
  /*                       (struct sockaddr *) incoming_addr, */
  /*                       (socklen_t *) sizeof(struct sockaddr_in)); */

  /* list_insert(server_conn->clients, client); */
}
