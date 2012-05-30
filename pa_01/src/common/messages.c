#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "messages.h"
#include "output.h"

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
  return NULL; /* TODO */
}

size_t client_message_write(client_message_t * msg, char * buf)
{
  size_t len;

  switch(msg->type) {
  case CL_CON_REQ:
    len = 1 + 4 + strlen(msg->cl_con_req.name) + 1;
    buf = calloc(1, len);
    sprintf(buf,
            "%1d%2d%s",
            msg->type,
            htons(strlen(msg->cl_con_req.name)),
            msg->cl_con_req.name);
    break;

  case CL_ROOM_MSG:
    len = 1 + 4 + strlen(msg->cl_room_msg.room_name) + 1 + 1;
    buf = calloc(1, len);
    sprintf(buf,
            "%1d%2d%s%1d",
            msg->type,
            htons(strlen(msg->cl_room_msg.room_name)),
            msg->cl_room_msg.room_name,
            msg->cl_room_msg.action);
    break;

  case CL_MSG:
    len = 1 + 4 + strlen(msg->cl_msg.room_name) + 1 + strlen(msg->cl_msg.message) + 1;
    buf = calloc(1, len);
    sprintf(buf,
            "%1d%2d%s%2d%s",
            msg->type,
            htons(strlen(msg->cl_msg.room_name)),
            msg->cl_msg.room_name,
            htons(strlen(msg->cl_msg.room_name)),
            msg->cl_msg.message);
    break;

  case CL_DISC_REQ:
    len = 1;
    buf = calloc(1, len);
    sprintf(buf, "%1d", msg->type);
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
    error(false, "Could not allocate memory for a message!");

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
