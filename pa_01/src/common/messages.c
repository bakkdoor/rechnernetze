#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "messages.h"
#include "output.h"
#include "network_conversations.h"

void client_message_delete(client_message_t * client_message)
{
  /* TODO */
}

void _client_message_delete(void * client_message)
{
  client_message_delete((client_message_t *)client_message);
}

client_message_t * client_message_read(char * buf)
{
  char type;
  char * tmp;
  unsigned int length = 0;
  client_message_t * message = calloc(1, sizeof(client_message_t));

  if(!message)
    error(false, "Could not allocate memory for a client message!");

  tmp = (char *) memcpy(&type, buf, 1);
  type = *tmp;

  message->type = type;
  buf++;

  /* TODO: */
  switch(type) {
  case CL_CON_REQ:
    memcpy(&length, buf, 4);
    buf += 4;
    message->cl_con_req.length = ntohs(length);
    memcpy(message->cl_con_req.name, buf, length);
    break;

  case CL_ROOM_MSG:
    memcpy(&length, buf, 4);
    buf += 4;
    message->cl_room_msg.length = ntohs(length);
    memcpy(message->cl_room_msg.room_name, buf, length);
    buf += length;
    memcpy(&message->cl_room_msg.action, buf, 1);
    break;

  case CL_MSG:
    memcpy(&length, buf, 4);
    buf += 4;
    message->cl_msg.room_length = ntohs(length);
    memcpy(message->cl_msg.room_name, buf, length);
    buf += length;

    memcpy(&length, buf, 4);
    buf += 4;
    message->cl_msg.msg_length = ntohs(length);
    memcpy(message->cl_msg.message, buf, length);
    buf += length;
    break;

  case CL_DISC_REQ:
    break;

  default:
    error(false, "Unknown incoming message type: %d", type);
    assert(0);
    break;
  }

  return message;
}

size_t client_message_write(client_message_t * msg, char * buf)
{
  size_t len;
  unsigned int net_length;
  char * name;

  memcpy(buf++, &msg->type, 1);
  len = 1;

  switch(msg->type) {
  case CL_CON_REQ:
    len += 4 + msg->cl_con_req.length;

    net_length = htons(msg->cl_con_req.length);
    name = str_to_net(msg->cl_con_req.name);

    memcpy(buf, &net_length, 4);
    buf += 4;
    memcpy(buf, name, msg->cl_con_req.length);
    buf += msg->cl_con_req.length;

    free(name);

    break;

  case CL_ROOM_MSG:
    len += 4 + msg->cl_room_msg.length + 1;
    net_length = htons(msg->cl_room_msg.length);
    memcpy(buf, &net_length, 4);
    buf += 4;
    memcpy(buf, &msg->cl_room_msg.action, 1);
    break;

  case CL_MSG:
    len += 4 + strlen(msg->cl_msg.room_name) + 1 + strlen(msg->cl_msg.message) + 1;

    net_length = htons(msg->cl_msg.room_length);
    name = str_to_net(msg->cl_msg.room_name);
    memcpy(buf, &net_length, 4);
    buf += 4;
    memcpy(buf, name, msg->cl_msg.room_length);
    buf += msg->cl_msg.room_length;

    free(name);

    net_length = htons(msg->cl_msg.msg_length);
    name = str_to_net(msg->cl_msg.message);
    memcpy(buf, &net_length, 4);
    buf += 4;
    memcpy(buf, name, msg->cl_msg.msg_length);
    buf += msg->cl_msg.msg_length;

    free(name);

    break;

  case CL_DISC_REQ:
    /* all done. only send msg id which is at beginning */
    break;

  default:
    error(false, "Unknown message type: %d", msg->type);
    assert(0);
    break;
  }

  return len;
}

void server_message_delete(server_message_t * server_message) {
  if (!server_message) return;
  switch(server_message->type) {
    case SV_CON_REP:
      break;

    case SV_ROOM_MSG: break;
    case SV_AMSG: break;
    case SV_DISC_REP: break;
    case SV_DISC_AMSG: break;
    default: break;
  }

  free(server_message);
}

void _server_message_delete(void * server_message)
{
  server_message_delete((server_message_t *)server_message);
}

server_message_t * server_message_read(char * buf)
{
  char type;
  char * tmp;
  server_message_t * message = calloc(1, sizeof(server_message_t));

  if(!message)
    error(false, "Could not allocate memory for a server message!");

  tmp = (char *) memcpy(&type, buf, 1);
  type = *tmp;

  /* TODO: */
  switch(type) {
  case SV_CON_REP:
    message->type = type;
    memcpy(&message->sv_con_rep.state, buf + 1, sizeof(char));
    memcpy(&message->sv_con_rep.comm_port, buf + 1 + sizeof(char), sizeof(int));
    message->sv_con_rep.comm_port = ntohs(message->sv_con_rep.comm_port);
    break;

  case SV_ROOM_MSG:
    break;

  case SV_AMSG:
    break;

  case SV_DISC_REP:
    break;

  case SV_DISC_AMSG:
    break;

  default:
    error(false, "Unknown incoming message type: %d", type);
    assert(0);
    break;
  }

  return message;
}

size_t server_message_write(server_message_t * server_message, char * buf)
{
  return 0; /* TODO */
}
