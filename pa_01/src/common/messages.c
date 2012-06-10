#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "messages.h"
#include "output.h"

/* HELPER FUNCTIONS  */

uint32_t read_int(char * buf, size_t n_bytes)
{
  unsigned int length;
  memcpy(&length, buf, n_bytes);
  if(n_bytes == 1) {
    return length;
  }
  return ntohl(length);
}

uint8_t read_byte(char * buf)
{
  uint8_t byte;
  memcpy(&byte, buf, 1);
  return byte;
}

char * read_string(char * buf, size_t length)
{
  char * tmp = calloc(length + 1, sizeof(char));
  memcpy(tmp, buf, length);
  tmp[length] = '\0';
  return tmp;
}

void write_int(char * buf, unsigned int i, size_t n_bytes)
{
  unsigned int net_int = htonl(i);
  if(n_bytes == 1) {
    net_int = i;
  }
  memcpy(buf, &net_int, n_bytes);
}

void write_byte(char * buf, uint8_t i)
{
  memcpy(buf, &i, 1);
}

void write_string(char * buf, char * str, size_t length)
{
  memcpy(buf, str, length);
}

/* MAIN MESSAGE FUNCTIONS */

void client_message_delete(client_message_t * msg)
{
  switch(msg->type) {
  case CL_CON_REQ:
    free(msg->cl_con_req.name);
    break;

  case CL_ROOM_MSG:
    free(msg->cl_room_msg.room_name);
    break;

  case CL_MSG:
    free(msg->cl_msg.room_name);
    free(msg->cl_msg.message);
    break;

  case CL_DISC_REQ:
    break;

  default:
    break;
  }

  free(msg);
}

void _client_message_delete(void * client_message)
{
  client_message_delete((client_message_t *)client_message);
}

client_message_t * client_message_read(char * buf)
{
  unsigned int length = 0;
  client_message_t * message = calloc(1, sizeof(client_message_t));

  if(!message) {
    error(false, "Could not allocate memory for a client message!");
    return NULL;
  }

  message->type = read_byte(buf);
  buf++;

  /* TODO: */
  switch(message->type) {
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

    message->cl_room_msg.action = read_byte(buf);
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
    error(false, "Unknown incoming message type: %u", message->type);
    assert(0);
    break;
  }

  return message;
}

size_t client_message_write(client_message_t * msg, char * buf)
{
  char * buf_start = buf;

  memcpy(buf++, &msg->type, 1);

  switch(msg->type) {
  case CL_CON_REQ:
    write_int(buf, msg->cl_con_req.length, 4);
    buf += 4;

    write_string(buf, msg->cl_con_req.name, msg->cl_con_req.length);
    buf += msg->cl_con_req.length;
    break;

  case CL_ROOM_MSG:
    write_int(buf, msg->cl_room_msg.length, 4);
    buf += 4;

    write_string(buf, msg->cl_room_msg.room_name, msg->cl_room_msg.length);
    buf += msg->cl_room_msg.length;

    write_byte(buf, msg->cl_room_msg.action);
    buf++;
    break;

  case CL_MSG:
    write_int(buf, msg->cl_msg.room_length, 4);
    buf += 4;

    write_string(buf, msg->cl_msg.room_name, msg->cl_msg.room_length);
    buf += msg->cl_msg.room_length;

    write_int(buf, msg->cl_msg.msg_length, 4);
    buf += 4;

    write_string(buf, msg->cl_msg.message, msg->cl_msg.msg_length);
    buf += msg->cl_msg.msg_length;
    break;

  case CL_DISC_REQ:
    /* all done. only send msg id which is at beginning */
    break;

  default:
    error(false, "Unknown message type: %d", msg->type);
    assert(0);
    break;
  }


  return buf - buf_start; /* total message length in bytes */
}

void server_message_delete(server_message_t * msg) {
  if (!msg) return;
  switch(msg->type) {
    case SV_CON_REP:
      break;

    case SV_ROOM_MSG:
      free(msg->sv_room_msg.room);
      free(msg->sv_room_msg.user);
      break;

    case SV_AMSG:
      free(msg->sv_amsg.room);
      free(msg->sv_amsg.user);
      free(msg->sv_amsg.msg);
      break;

    case SV_DISC_REP:
      break;

    case SV_DISC_AMSG:
      free(msg->sv_disc_amsg.user);
      break;

    default:
      break;
  }

  free(msg);
}

void _server_message_delete(void * server_message)
{
  server_message_delete((server_message_t *)server_message);
}

server_message_t * server_message_read(char * buf)
{
  unsigned int length = 0;
  server_message_t * message = calloc(1, sizeof(server_message_t));

  if(!message) {
    error(false, "Could not allocate memory for a server message!");
    return NULL;
  }

  message->type = read_byte(buf);
  buf++;

  /* TODO: */
  switch(message->type) {
  case SV_CON_REP:
    message->sv_con_rep.state = read_byte(buf);
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

    message->sv_room_msg.action = read_byte(buf);
    buf++;
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
    error(false, "Unknown incoming message type: %u", message->type);
    assert(0);
    break;
  }

  return message;
}

size_t server_message_write(server_message_t * msg, char * buf)
{
  char * buf_start = buf;
  memcpy(buf, &msg->type, 1);
  buf++;

  switch(msg->type) {
  case SV_CON_REP:
    write_byte(buf, msg->sv_con_rep.state);
    buf++;
    write_int(buf, msg->sv_con_rep.comm_port, 4);
    buf += 4;
    break;

  case SV_ROOM_MSG:
    write_int(buf, msg->sv_room_msg.room_length, 4);
    buf += 4;

    write_string(buf, msg->sv_room_msg.room, msg->sv_room_msg.room_length);
    buf += msg->sv_room_msg.room_length;

    write_int(buf, msg->sv_room_msg.user_length, 4);
    buf += 4;

    write_string(buf, msg->sv_room_msg.user, msg->sv_room_msg.user_length);
    buf += msg->sv_room_msg.user_length;

    write_byte(buf, msg->sv_room_msg.action);
    buf++;
    break;

  case SV_AMSG:
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
    write_int(buf, msg->sv_disc_amsg.user_length, 4);
    buf += 4;

    write_string(buf, msg->sv_disc_amsg.user, msg->sv_disc_amsg.user_length);
    buf += msg->sv_disc_amsg.user_length;
    break;
  }

  return buf - buf_start;
}
