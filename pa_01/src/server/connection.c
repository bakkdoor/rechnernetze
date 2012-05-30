#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>


#include "connection.h"
#include "client.h"
#include "../common/list.h"
#include "../common/chat_room.h"
#include "../common/output.h"
#include "../common/chat_user.h"

#define DEFAULT_TIMEOUT_USEC 1000

struct server_connection {
  int sock;
  int port;
  struct sockaddr_in * addr;
  list_t  * rooms;
  list_t  * clients;
  int clients_nfds;
  int last_port;
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
  server_conn->last_port = port;

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
  char * buf;
  int bytes_read;
  client_message_t * message = NULL;
  client_t * client;
  chat_user_t * chat_user;
  int client_comm_port;
  struct sockaddr_in * client_addr = calloc(1, sizeof(struct sockaddr_in));


  buf = calloc(MAX_CLIENT_MSG_SIZE, sizeof(char));

  bytes_read = recvfrom(server_conn->sock, buf, sizeof(buf), 0,
                        (struct sockaddr *) client_addr,
                        (socklen_t *) sizeof(struct sockaddr_in));

  if(bytes_read < 1) {
    free(buf);
    free(client_addr);
    return;
  }

  message = client_message_read(buf);
  free(buf);

  if(message->type != CL_CON_REQ) {
    error(false, "Invalid initial request type: %d", message->type);
    return;
  }

  /*
     create new user & client and add them to server_conn's clients list
  */

  client_comm_port = ++server_conn->last_port;

  chat_user = chat_user_new(message->cl_con_req.name);
  client = client_new(chat_user, client_addr, client_comm_port);
  list_insert(server_conn->clients, client);

  /* send reply to client with new port number */

  server_message_t reply;
  reply.type = SV_CON_REP;
  reply.sv_con_rep.state = htonl(CON_REP_OK);
  reply.sv_con_rep.comm_port = htonl(client_comm_port);

  client_send_message(client, &reply);

  client_message_delete(message);
  free(buf);
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
    list_delete(clients_with_data, NULL);
    list_delete(incoming_messages, client_message_delete);
  }
}

void server_connection_handle_incoming(server_connection_t * server_conn)
{
  server_connection_handle_new_clients(server_conn);
  server_connection_handle_client_messages(server_conn);
}
