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

#define DEFAULT_TIMEOUT_USEC 1000

struct server_connection {
  int sock;
  int port;
  struct sockaddr_in * addr;
  list_t  * rooms;
  list_t  * clients;
  int clients_nfds;
};

static server_connection_t * _server_connection = NULL;

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

  _server_connection = server_conn;

  return server_conn;
}

void server_connection_delete(server_connection_t * server_conn)
{
  if(!server_conn)
    return;

  server_connection_close(server_conn);

  list_delete(server_conn->rooms, chat_room_delete);
  list_delete(server_conn->clients, client_delete);

  free(server_conn->addr);
  free(server_conn);
}

bool server_connection_close(server_connection_t * server_conn)
{
  if(!server_conn)
    return false;

  return close(server_conn->sock);
}

void server_connection_handle_new_clients(server_connection_t * server_conn)
{
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

static fd_set _read_fds;
void client_fd_set(void * _client)
{
  client_t * client = _client;
  FD_SET(client->sock, &_read_fds);
}

bool client_in_fdset(const void * _client)
{
  const client_t * client = _client;
  return FD_ISSET(client->sock, &_read_fds);
}

client_message_t * client_handle_incoming(const client_t * client)
{
  char * buf;
  int bytes_read;
  client_message_t * incoming_message = NULL;

  buf = calloc(MAX_CLIENT_MSG_SIZE, sizeof(char));

  bytes_read = recvfrom(client->sock, buf, sizeof(buf), 0,
                        (struct sockaddr *) client->addr,
                        (socklen_t *) sizeof(struct sockaddr_in));

  if(bytes_read < 1) {
    free(buf);
    return NULL;
  }

  incoming_message = client_message_read(buf);
  free(buf);
  return incoming_message;
}

void * _client_handle_incoming(const void * client)
{
  return client_handle_incoming((client_t *)client);
}

void server_connection_handle_message(client_message_t * client_message)
{
 /*
    TODO:
    dispatch logic (send messages to all users in same room etc
 */
}

void _server_connection_handle_message(void * client_message)
{
  server_connection_handle_message((client_message_t *)client_message);
}

void server_connection_handle_client_messages(server_connection_t * server_conn)
{
  int ready_socks;
  struct timeval timeout;

  timeout.tv_sec = 0;
  timeout.tv_usec = DEFAULT_TIMEOUT_USEC;

  FD_ZERO(&_read_fds);
  list_foreach(server_conn->clients, client_fd_set);

  ready_socks = select(server_conn->clients_nfds, &_read_fds, NULL, NULL, &timeout);

  if(ready_socks > 0) {
    list_t * clients_with_data = list_filter(server_conn->clients, client_in_fdset);
    list_t * incoming_messages = list_map(clients_with_data, _client_handle_incoming);
    list_foreach(incoming_messages, _server_connection_handle_message);
  }
}

void server_connection_handle_incoming(server_connection_t * server_conn)
{
  server_connection_handle_new_clients(server_conn);
  server_connection_handle_client_messages(server_conn);
}
