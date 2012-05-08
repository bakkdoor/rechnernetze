#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connection.h"

int connection_setup(int port)
{
  /* TODO */
  /* int sockfd; */
  return 0;
}

int connection_close(int sockfd)
{
  return close(sockfd);
}

int connection_send_client_message(int sock, client_message_t * msg)
{
  char * buf;
  size_t len;
  int bytes_read;

  switch(msg->type) {
  case CL_CON_REQ:
    len = 1 + 4 + strlen(msg->cl_con_req.name) + 1;
    buf = calloc(1, len);
    sprintf(buf, "%1d%2d%s", msg->type, htons(strlen(msg->cl_con_req.name)), msg->cl_con_req.name);
    break;
  case CL_ROOM_MSG:
    len = 1 + 4 + strlen(msg->cl_room_msg.room_name) + 1 + 1;
    buf = calloc(1, len);
    sprintf(buf, "%1d%2d%s%1d", msg->type, htons(strlen(msg->cl_room_msg.room_name)), msg->cl_room_msg.room_name, msg->cl_room_msg.action);
    break;
  case CL_MSG:
    len = 1 + 4 + strlen(msg->cl_msg.room_name) + 1 + strlen(msg->cl_msg.message) + 1;
    buf = calloc(1, len);
    sprintf(buf, "%1d%2d%s%2d%s", msg->type, htons(strlen(msg->cl_msg.room_name)), msg->cl_msg.room_name, htons(strlen(msg->cl_msg.room_name)), msg->cl_msg.message);
    break;
  case CL_DISC_REQ:
    len = 1;
    buf = calloc(1, len);
    sprintf(buf, "%1d", msg->type);
    break;
  default:
    fprintf(stderr, "Unknown message type: %d", msg->type);
    assert(0);
    break;
  }

  bytes_read = write(sock, buf, len);

  free(buf); /* ?!?! TODO: Check if OK here. */

  return bytes_read;
}
