#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "connection.h"
#include "client.h"
#include "../common/list.h"
#include "../common/chat_room.h"
#include "../common/output.h"
#include "../common/chat_user.h"

#define DEFAULT_TIMEOUT_USEC 1000

struct server_connection {
  int sock;
  unsigned int port;
  struct sockaddr_in * addr;
  list_t  * rooms;
  list_t  * clients;
  int clients_nfds;
};

typedef struct client_with_message {
  const client_t * client;
  client_message_t * msg;
} client_with_message_t;

void _client_with_message_delete(void * _cwm)
{
  client_with_message_t * cwm = _cwm;
  if(!cwm)
    return;

  client_message_delete(cwm->msg);
  free(cwm);
}

static server_connection_t * _server_connection = NULL;

////////////////////////////////////////////////////////

server_connection_t * server_connection_new(int port)
{
  int sockfd, err;
  struct sockaddr_in * addr = calloc(1, sizeof(struct sockaddr_in));
  server_connection_t * server_conn = calloc(1, sizeof(server_connection_t));

  if(!addr) {
    error(true, "Could not setup sockaddr_in struct");
  }

  if(!server_conn)
    return NULL;

  sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sockfd < 0) {
    error(true, "Could not setup server socket");
  }

  server_conn->sock = sockfd;
  server_conn->port = port;

  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  addr->sin_addr.s_addr = htonl(INADDR_ANY);

  err = bind(sockfd, (struct sockaddr *) addr, sizeof(struct sockaddr_in));
  if(err < 0) {
    perror("bind()");
    error(true, "Could not bind on port");
  }

  server_conn->addr = addr;

  server_conn->rooms = list_new();
  server_conn->clients = list_new();

  fcntl(server_conn->sock, F_SETFL, O_NONBLOCK);

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
  int slen = sizeof(struct sockaddr_in);
  int tmp_sock;
  struct sockaddr_in * client_addr = calloc(1, sizeof(struct sockaddr_in));
  struct sockaddr_in * tmp_addr;
  buf = calloc(MAX_CLIENT_MSG_SIZE, sizeof(char));

  bytes_read = recvfrom(server_conn->sock, buf, MAX_CLIENT_MSG_SIZE, 0,
                        (struct sockaddr *) client_addr,
                        (socklen_t *) &slen);

  if(bytes_read < 1) {
    free(buf);
    free(client_addr);
    return;
  }

  message = client_message_read(buf);
  free(buf);

  if(message->type != CL_CON_REQ) {
    error(false, "Invalid initial request type: %d", message->type);
    client_message_delete(message);
    return;
  }

  /*
     create new user & client and add them to server_conn's clients list
  */

  chat_user = chat_user_new(message->cl_con_req.name);
  client = client_new(chat_user, client_addr);

  if(!client) {
    free(client_addr);
    free(buf);
    return;
  }

  list_insert(server_conn->clients, client);

  info("New user connected with name: %s", chat_user->name);

  /* send reply to client with new port number */

  server_message_t reply;
  reply.type = SV_CON_REP;
  reply.sv_con_rep.state = CON_REP_OK;
  reply.sv_con_rep.comm_port = client->port;

  buf = calloc(1, MAX_SERVER_MSG_SIZE);
  if(!buf) {
    error(false, "Could not allocate reponse buffer");
    client_message_delete(message);
    return;
  }

  /* set client sock to server sock temporarily
     so we can send the connect reply message back to where the client
     is expecting it.
   */
  tmp_sock = client->sock;
  tmp_addr = client->addr;
  client->addr = client_addr;
  client->sock = server_conn->sock;
  client_send_message(client, &reply);
  client->sock = tmp_sock;
  client->addr = tmp_addr;

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

static char * _room_name;
bool _room_with_name(const void * _room)
{
  const chat_room_t * room = _room;
  return strcmp(room->name, _room_name) == 0;
}

void server_connection_handle_message(server_connection_t * server_conn, client_with_message_t * cwm)
{
 /*
    TODO:
    dispatch logic (send messages to all users in same room etc
 */

  client_message_t * msg = cwm->msg;
  const client_t * client = cwm->client;
  chat_room_t * room;
  server_message_t * reply;
  list_node_t * current;

  info("Got message with type: %d", cwm->msg->type);

  reply = calloc(1, sizeof(server_message_t));
  if(!reply) {
    error(false, "Could not create reply message");
    return;
  }

  switch(msg->type) {
  case CL_ROOM_MSG:
    _room_name = msg->cl_room_msg.room_name;
    info("User %s joining room %s", client->chat_user->name, _room_name);
    room = list_find_first(server_conn->rooms, _room_with_name);
    if(room) {
      list_insert(client->chat_user->rooms, room);
      list_insert(server_conn->rooms, room);
    } else {
      room = chat_room_new(msg->cl_room_msg.room_name);

      if(!room) {
        error(false, "Could not create new chatroom with name: %s", msg->cl_room_msg.room_name);
        return;
      }

      chat_room_add_user(room, client->chat_user);
      list_insert(server_conn->rooms, room);
      list_insert(client->chat_user->rooms, room);
    }

    reply->type = SV_ROOM_MSG;

    reply->sv_room_msg.room_length = strlen(room->name) + 1;
    reply->sv_room_msg.room = calloc(reply->sv_room_msg.room_length, sizeof(char));
    strcpy(reply->sv_room_msg.room, room->name);

    reply->sv_room_msg.user_length = strlen(client->chat_user->name) + 1;
    reply->sv_room_msg.user = calloc(reply->sv_room_msg.user_length, sizeof(char));
    strcpy(reply->sv_room_msg.user, client->chat_user->name);

    server_connection_room_broadcast(server_conn, reply, room->name);
    break;

  case CL_MSG:
    reply->type = SV_AMSG;

    reply->sv_amsg.room_length = msg->cl_msg.room_length;
    reply->sv_amsg.room = calloc(reply->sv_amsg.room_length, sizeof(char));
    strcpy(reply->sv_amsg.room, msg->cl_msg.room_name);

    reply->sv_amsg.user_length = strlen(client->chat_user->name) + 1;
    reply->sv_amsg.user = calloc(reply->sv_amsg.user_length, sizeof(char));
    strcpy(reply->sv_amsg.user, client->chat_user->name);

    reply->sv_amsg.msg_length = msg->cl_msg.msg_length;
    reply->sv_amsg.msg = calloc(reply->sv_amsg.msg_length, sizeof(char));
    strcpy(reply->sv_amsg.msg, msg->cl_msg.message);

    server_connection_room_broadcast(server_conn, reply, msg->cl_msg.room_name);
    break;

  case CL_DISC_REQ:
    reply->type = SV_DISC_REP;
    client_send_message(client, reply);

    /* todo: sent disc_amsg to all rooms of client */
    server_message_delete(reply);

    reply = calloc(1, sizeof(server_message_t));
    reply->type = SV_DISC_AMSG;

    reply->sv_disc_amsg.user_length = strlen(client->chat_user->name) + 1;
    reply->sv_disc_amsg.user = calloc(reply->sv_disc_amsg.user_length, sizeof(char));
    strcpy(reply->sv_disc_amsg.user, client->chat_user->name);

    current = client->chat_user->rooms->first;

    /* remove user from rooms */
    for(; current; current = current->next) {
      room = current->data;
      server_connection_room_broadcast(server_conn, reply, room->name);
      list_remove(room->users, (chat_user_t *)client->chat_user, true, NULL, NULL); /* remove from room */
    }

    /* remove user from list of clients in server_conn */
    list_remove(server_conn->clients, (client_t *)client, true, NULL, NULL);

    client_delete((client_t*)client);
    break;

  default:
    break;

    server_message_delete(reply);
  }
}

void _server_connection_handle_message(void * client_with_message)
{
  server_connection_handle_message(_server_connection, (client_with_message_t *)client_with_message);
}

void * _client_read_message(const void * _client)
{
  const client_t * client = _client;
  client_with_message_t  * cwm = calloc(1, sizeof(client_with_message_t));
  cwm->msg = client_read_message(client);
  cwm->client = client;
  return cwm;
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
    info("got sockets: %d", ready_socks);
    list_t * clients_with_data = list_filter(server_conn->clients, client_in_fdset);
    list_t * incoming_messages = list_map(clients_with_data, _client_read_message);
    list_foreach(incoming_messages, _server_connection_handle_message);
    list_delete(clients_with_data, NULL);
    list_delete(incoming_messages, _client_with_message_delete);
  }
}

void server_connection_handle_incoming(server_connection_t * server_conn)
{
  server_connection_handle_new_clients(server_conn);
  server_connection_handle_client_messages(server_conn);
}

void server_connection_room_broadcast(server_connection_t * server_conn, server_message_t * msg, char * room)
{
  client_t * client;
  list_node_t * node = server_conn->clients->first;
  for(; node; node = node->next) {
    client = node->data;
    if(chat_user_in_room(client->chat_user, room)) {
      client_send_message(client, msg);
    }
  }
}
