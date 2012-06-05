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

void help(char * progname)
{
  puts("Usage:");
  printf("%s <server_addr> <server_port> <name>\n", progname);
}

int main(int argc, char ** argv)
{
  char input[1500], /* tmp[1500], */ format[1500];
  client_connection_t * cli_conn;
  client_message_t * msg;

  if(argc < 4) {
    help(argv[0]);
    return 1;
  }

  cli_conn = connection_setup(argv[1], argv[2], argv[3]);
  if (!cli_conn) {
    return 1;
  }
/*

  for (;fscanf(stdin, CHAT_CMD_JOIN, input);) {
    sprintf(format, CHAT_CMD_JOIN, "roomname");
    printf("[HELP]: Join a channel with %s!\n", format);
  }

  msg = calloc(1, sizeof(client_message_t));
  if (!msg) {
    //TODO
  }

  msg->type = CL_ROOM_MSG;
  msg->cl_room_msg.action = CL_ROOM_MSG_ACTION_JOIN;
  msg->cl_room_msg.room_name = input;
  msg->cl_room_msg.length = strlen(input);

  if (connection_send_client_message(cli_conn, msg) < 1) {
    //TODO can't send msg
  }

  strcpy(input, "");
  // TODO handle masseges
  while (1) {
    readline(stdin, input, sizeof(input));
    if (strcmp(CHAT_CMD_DISCONNECT, input) == 0) {
      goto quit;
    } else if (input_is_join(input)) {
      // TODO
    } else if (input_is_leave(input)) {

    } else {
      // std message
    }
  }
*/

quit:
  connection_close(cli_conn);
  return 0;
}

int input_is_join(const char * input) {
  return scanf(CHAT_CMD_JOIN, input);
}

int input_is_leave(const char * input) {
  return scanf(CHAT_CMD_LEAVE, input);
}
