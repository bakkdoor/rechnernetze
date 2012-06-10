#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "../common/list.h"
#include "../common/output.h"
#include "connection.h"
#include "client.h"
#include "chat_room.h"

#define DEFAULT_TIMEOUT_USEC 1000

struct server_connection {
  int sock;
  in_port_t port;
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

server_connection_t * server_connection_new(in_port_t port)
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

  server_conn->clients_nfds = sockfd;

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

  client = client_new(message->cl_con_req.name);

  if(client->sock > server_conn->clients_nfds)
    server_conn->clients_nfds = client->sock;

  if(!client) {
    free(client_addr);
    free(buf);
    return;
  }

  info("New user connected with name: %s", client->name);

  /* send reply to client with new port number */

  server_message_t reply;
  reply.type = SV_CON_REP;


  if(server_connection_has_client(server_conn, client)) {
    info("User tried to connect with already taken name: %s", client->name);
    reply.sv_con_rep.state = CON_REP_BAD_USERNAME;
    reply.sv_con_rep.comm_port = 0;
  } else {
    reply.sv_con_rep.state = CON_REP_OK;
    reply.sv_con_rep.comm_port = client->port;
    list_insert(server_conn->clients, client);
  }

  buf = calloc(1, MAX_SERVER_MSG_SIZE);
  if(!buf) {
    error(false, "Could not allocate reponse buffer");
    free(client_addr);
    client_message_delete(message);
    client_delete(client);
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
  free(client_addr);

  if(reply.sv_con_rep.state == CON_REP_BAD_USERNAME) {
    close(client->sock);
    client_delete(client);
  }
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

void server_connection_add_room(server_connection_t * server_conn, chat_room_t * room)
{
  list_insert(server_conn->rooms, room);
}

chat_room_t * server_connection_find_room(server_connection_t * server_conn, char * room_name)
{
  _room_name = room_name;
  return list_find_first(server_conn->rooms, _room_with_name);
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
    room = server_connection_find_room(server_conn, msg->cl_room_msg.room_name);

    if(room) {
      reply->type = SV_ROOM_MSG;
      reply->sv_room_msg.action = msg->cl_room_msg.action;

      reply->sv_room_msg.room_length = strlen(room->name);
      reply->sv_room_msg.room = calloc(reply->sv_room_msg.room_length, sizeof(char));
      memcpy(reply->sv_room_msg.room, room->name, reply->sv_room_msg.room_length);

      reply->sv_room_msg.user_length = strlen(client->name);
      reply->sv_room_msg.user = calloc(reply->sv_room_msg.user_length, sizeof(char));
      memcpy(reply->sv_room_msg.user, client->name, reply->sv_room_msg.user_length);

      server_connection_room_broadcast(server_conn, reply, room);
    }

    /* check for action and remove client from chat_room if action == CL_ROOM_MSG_ACTION_LEAVE */

    switch(msg->cl_room_msg.action) {
    case CL_ROOM_MSG_ACTION_JOIN:
      info("User %s joining room %s", client->name, _room_name);

      if(!room) {
        info("Creating new room: %s", msg->cl_room_msg.room_name);
        room = chat_room_new(msg->cl_room_msg.room_name);

        if(!room) {
          error(false, "Could not create new chatroom with name: %s", msg->cl_room_msg.room_name);
          break;
        }

        server_connection_add_room(server_conn, room);
      }

      client_join_room((client_t *)client, room);
      break;

    case CL_ROOM_MSG_ACTION_LEAVE:
      info("User %s leaving room %s", client->name, _room_name);
      if (room && chat_room_has_client(room, client)) {
        client_leave_room((client_t *)client, room);
      }
      break;

    default:
      break;
    }

    break;

  case CL_MSG:
    reply->type = SV_AMSG;

    info("incoming message: %s (on room: %s)", msg->cl_msg.message, msg->cl_msg.room_name);
    reply->sv_amsg.room_length = msg->cl_msg.room_length;
    reply->sv_amsg.room = calloc(reply->sv_amsg.room_length, sizeof(char));
    memcpy(reply->sv_amsg.room, msg->cl_msg.room_name, reply->sv_amsg.room_length);

    reply->sv_amsg.user_length = strlen(client->name);
    reply->sv_amsg.user = calloc(reply->sv_amsg.user_length, sizeof(char));
    memcpy(reply->sv_amsg.user, client->name, reply->sv_amsg.user_length);

    reply->sv_amsg.msg_length = msg->cl_msg.msg_length;
    reply->sv_amsg.msg = calloc(reply->sv_amsg.msg_length, sizeof(char));
    memcpy(reply->sv_amsg.msg, msg->cl_msg.message, reply->sv_amsg.msg_length);

    room = server_connection_find_room(server_conn, msg->cl_msg.room_name);
    if(!room) {
      error(false, "Could not send message to non-existant room: %s", msg->cl_msg.room_name);
      break;
    }

    if(chat_room_has_client(room, client)) {
      server_connection_room_broadcast(server_conn, reply, room);
      info("Broadcasting to room %s : %s", msg->cl_msg.room_name, msg->cl_msg.message);
    } else {
      warn("User %s tried to send message to room: %s without being in it", client->name, room->name);
    }
    break;

  case CL_DISC_REQ:
    reply->type = SV_DISC_REP;
    client_send_message(client, reply);

    /* todo: sent disc_amsg to all rooms of client */
    server_message_delete(reply);

    reply = calloc(1, sizeof(server_message_t));
    reply->type = SV_DISC_AMSG;

    reply->sv_disc_amsg.user_length = strlen(client->name);
    reply->sv_disc_amsg.user = calloc(reply->sv_disc_amsg.user_length, sizeof(char));
    memcpy(reply->sv_disc_amsg.user, client->name, reply->sv_disc_amsg.user_length);

    current = client->rooms->first;

    /* remove user from rooms */
    for(; current; current = current->next) {
      room = current->data;
      server_connection_room_broadcast(server_conn, reply, room);
      list_remove(room->clients, (client_t *)client, true, NULL, NULL); /* remove from room */
    }

    /* remove user from list of clients in server_conn */
    list_remove(server_conn->clients, (client_t *)client, true, NULL, NULL);

    client_delete((client_t*)client);
    break;

  default:
    break;
  }

  server_message_delete(reply);
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
  list_t * clients_with_data;
  list_t * incoming_messages;

  timeout.tv_sec = 0;
  timeout.tv_usec = DEFAULT_TIMEOUT_USEC;

  FD_ZERO(&_read_fds);
  list_foreach(server_conn->clients, client_fd_set);

  ready_socks = select(server_conn->clients_nfds + 1, &_read_fds, NULL, NULL, &timeout);

  if(ready_socks > 0) {
    clients_with_data = list_filter(server_conn->clients, client_in_fdset);
    incoming_messages = list_map(clients_with_data, _client_read_message);
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

void server_connection_room_broadcast(server_connection_t * server_conn, server_message_t * msg, chat_room_t * room)
{
  client_t * client;
  list_node_t * node = server_conn->clients->first;
  for(; node; node = node->next) {
    client = node->data;
    if(chat_room_has_client(room, client)) {
      client_send_message(client, msg);
    } else {
      info("user: %s tried to broadcast to room although he is not in room: %s", client->name, room->name);
    }
  }
}

static char * _username;
bool _serv_conn_client_with_correct_name(const void * _client)
{
  const client_t * client = _client;
  return strcmp(_username, client->name) == 0;
}

bool server_connection_has_client(server_connection_t * server_conn, client_t * client)
{
  _username = client->name;
  if(list_find_first(server_conn->clients, _serv_conn_client_with_correct_name))
    return true;
  return false;
}
