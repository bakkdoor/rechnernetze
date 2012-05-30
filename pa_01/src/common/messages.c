#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "messages.h"
#include "output.h"
#include "network_conversations.h"


/* HELPER FUNCTIONS  */

unsigned int read_int(void * buf, size_t n_bytes)
{
  unsigned int length;
  memcpy(&length, buf, n_bytes);
  return ntohs(length);
}

char * read_string(void * buf, size_t length)
{
  char * str;
  char * tmp = calloc(1, length);
  memcpy(tmp, buf, length);
  str = net_to_str(tmp);
  free(tmp);
  return str;
}

void write_int(void * buf, int i, size_t n_bytes)
{
  unsigned int net_int = htons(i);
  memcpy(buf, &net_int, n_bytes);
}

void write_string(void * buf, char * str, size_t length)
{
  char * net_str = str_to_net(str);
  memcpy(buf, net_str, length);
  free(net_str);
}

/* MAIN MESSAGE FUNCTIONS */

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
    length = read_int(buf, 4);
    buf += 4;
    message->cl_con_req.length = length;

    message->cl_con_req.name = read_string(buf, length);
    buf += length;
    break;

  case CL_ROOM_MSG:
    length = read_int(buf, 4);
    buf += 4;
    message->cl_room_msg.length = length;

    message->cl_room_msg.room_name = read_string(buf, length);
    buf += length;

    message->cl_room_msg.action = read_int(buf, 1);
    break;

  case CL_MSG:
    length = read_int(buf, 4);
    buf += 4;
    message->cl_msg.room_length = length;

    message->cl_msg.room_name = read_string(buf, length);
    buf += length;

    length = read_int(buf, 4);
    buf += 4;
    message->cl_msg.msg_length = length;

    message->cl_msg.message = read_string(buf, length);
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
  unsigned int length;

  if(!message)
    error(false, "Could not allocate memory for a server message!");

  tmp = (char *) memcpy(&type, buf, 1);
  type = *tmp;
  message->type = type;
  buf++;

  /* TODO: */
  switch(type) {
  case SV_CON_REP:
    message->sv_con_rep.state = read_int(buf, 1);
    buf++;
    message->sv_con_rep.comm_port = read_int(buf, 4);
    buf += 4;
    break;

  case SV_ROOM_MSG:
    length = read_int(buf, 4);
    buf += 4;
    message->sv_room_msg.room_length = length;

    message->sv_room_msg.room = read_string(buf, length);
    buf += length;

    length = read_int(buf, 4);
    buf += 4;
    message->sv_room_msg.user_length = length;

    message->sv_room_msg.user = read_string(buf, length);
    buf += length;
    break;

  case SV_AMSG:
    length = read_int(buf, 4);
    buf += 4;
    message->sv_amsg.room_length = length;

    message->sv_amsg.room = read_string(buf, length);
    buf += length;

    length = read_int(buf, 4);
    buf += 4;
    message->sv_amsg.user_length = length;

    message->sv_amsg.user = read_string(buf, length);
    buf += length;

    length = read_int(buf, 4);
    buf += 4;
    message->sv_amsg.msg_length = length;

    message->sv_amsg.msg = read_string(buf, length);
    buf += length;
    break;

  case SV_DISC_REP:
    /* nothing to do */
    break;

  case SV_DISC_AMSG:
    length = read_int(buf, 4);
    buf += 4;
    message->sv_disc_amsg.user_length = length;

    message->sv_disc_amsg.user = read_string(buf, length);
    buf += length;
    break;

  default:
    error(false, "Unknown incoming message type: %d", type);
    assert(0);
    break;
  }

  return message;
}

size_t server_message_write(server_message_t * msg, char * buf)
{
  size_t len;
  unsigned int net_length;
  char * name;

  memcpy(buf++, &msg->type, 1);
  len = 1;

  switch(msg->type) {
  case SV_CON_REP:
    len += 1 + 4;
    write_int(buf, msg->sv_con_rep.state, 1);
    buf++;
    write_int(buf, msg->sv_con_rep.comm_port, 4);
    buf += 4;
    break;

  case SV_ROOM_MSG:
    len += 4 + msg->sv_room_msg.room_length + 4 + msg->sv_room_msg.user_length + 1;

    write_int(buf, msg->sv_room_msg.room_length, 4);
    buf += 4;

    write_string(buf, msg->sv_room_msg.room, msg->sv_room_msg.room_length);
    buf += msg->sv_room_msg.room_length;

    write_int(buf, msg->sv_room_msg.user_length, 4);
    buf += 4;

    write_string(buf, msg->sv_room_msg.user, msg->sv_room_msg.user_length);
    buf += msg->sv_room_msg.user_length;

    write_int(buf, msg->sv_room_msg.action, 1);
    buf++;
    break;

  case SV_AMSG:
    len += 4 + msg->sv_amsg.room_length + 4 + msg->sv_amsg.user_length + 4 + msg->sv_amsg.msg_length;

    write_int(buf, msg->sv_amsg.room_length, 4);
    buf += 4;

    write_string(buf, msg->sv_amsg.room, msg->sv_amsg.room_length);
    buf += msg->sv_amsg.room_length;

    write_int(buf, msg->sv_amsg.user_length, 4);
    buf += 4;

    write_string(buf, msg->sv_amsg.user, msg->sv_amsg.user_length);
    buf += msg->sv_amsg.user_length;

    write_int(buf, msg->sv_amsg.msg_length, 4);
    buf += 4;

    write_string(buf, msg->sv_amsg.msg, msg->sv_amsg.msg_length);
    buf += msg->sv_amsg.msg_length;
    break;

  case SV_DISC_REP:
    /* nothing to do */
    break;

  case SV_DISC_AMSG:
    len += 4 + msg->sv_disc_amsg.user_length;

    write_int(buf, msg->sv_disc_amsg.user_length, 4);
    buf += 4;

    write_string(buf, msg->sv_disc_amsg.user, msg->sv_disc_amsg.user_length);
    buf += msg->sv_disc_amsg.user_length;
    break;
  }

  return len;
}
