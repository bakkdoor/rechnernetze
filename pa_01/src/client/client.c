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
  
  msg = parse_client_message("/join test");
  if (!msg) {
    //TODO
  }
  
  connection_send_client_message(cli_conn, msg);
  
  while (true) {
    connection_handle_socks(cli_conn, 5);
  }
}
