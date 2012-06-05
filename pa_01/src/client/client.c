#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connection.h"
#include "../common/output.h"
#include "../common/unp_readline.h"

const char * CHAT_CMD_JOIN = "/join %s";
const char * CHAT_CMD_LEAVE = "/leave %s";
const char * CHAT_CMD_DISCONNECT = "/disconnect";
const char * CHAT_CMD_MSG = "%s: %s";

int input_is_join(const char * input);
int input_is_leave(const char * input);

void help(char * progname) {
  puts("Usage:");
  printf("%s <server_addr> <server_port> <name>\n", progname);
}

int main(int argc, char ** argv) {
  client_connection_t * cli_conn;
  client_message_t * msg;
  char buff[MAX_CLIENT_MSG_SIZE];

  if (argc < 4) {
    help(argv[0]);
    return 1;
  }

  cli_conn = connection_setup(argv[1], argv[2], argv[3]);
  if (!cli_conn) {
    return 1;
  }

  do {
    info("Choose a chat room with /join roomname.");

    if (readline(STDIN_FILENO, buff, sizeof (buff)) > 0) {
      msg = parse_client_message(buff);
      
      // exit loop if message is join or disconnect
      if (msg) {
        if (msg->type == CL_ROOM_MSG && msg->cl_room_msg.action == CL_ROOM_MSG_ACTION_JOIN) {
          break;
        } else if (msg->type == CL_DISC_REQ) {
          break;
        }
      }
    }
  } while (true);

  connection_send_client_message(cli_conn, msg);
  client_message_delete(msg);

  while (true) {
    connection_handle_socks(cli_conn, 5);
  }

  return 0;
}
