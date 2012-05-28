#include <stdlib.h>

#include "messages.h"

void server_message_delete(server_message_t * server_message) {
  if (!server_message) return;
  switch(server_message->type) {
    case SV_CON_REP: 
      free(server_message->sv_con_rep);
      break;
      
    case SV_ROOM_MSG: break;
    case SV_AMSG: break;
    case SV_DISC_REP: break;
    case SV_DISC_AMSG: break;
    default: break;
  }
  
  free(server_message);
}
