#include <stdio.h>
#include <stdlib.h>

#include "connection.h"
#include "../common/error.h"

void help(char * progname)
{
  puts("Usage:");
  printf("%s <server_addr> <server_port> <name>\n", progname);
}

int main(int argc, char ** argv)
{
  client_connection_t * cli_conn;
  
  if(argc < 4) {
    help(argv[0]);
    return 1;
  }
  
  cli_conn = connection_setup(argv[1], atoi(argv[2]), argv[3]);
  if (!cli_conn) {
    error("Coud not initalize connection!", true);
  }
  
  connection_close(cli_conn);
  return 0;
}
