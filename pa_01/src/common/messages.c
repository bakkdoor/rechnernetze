#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "messages.h"
#include "output.h"

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

int server_message_write(server_message_t * server_message, char * buf)
{
  return 0; /* TODO */
}
