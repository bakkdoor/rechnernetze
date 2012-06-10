#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../common/output.h"
#include "../common/list.h"
#include "chat_room.h"

#include "connection.h"

static list_t * chat_rooms;
static list_t * chat_clients;

void help(char * progname)
{
  puts("Usage:");
  printf("%s <server_port>\n", progname);
}

static bool Running;
static server_connection_t * ServerConn;
static int Port;

int main(int argc, char ** argv)
{
  if(argc == 1) {
    help(argv[0]);
    return 1;
  }

  Port = atoi(argv[1]);

  if(Port <= 0) {
    puts("Please provide a positive number for <server_port>");
    help(argv[0]);
    return 1;
  }

  chat_rooms = list_new();
  chat_clients = list_new();

  info("Chat Server - Listening on %d", Port);

  ServerConn = server_connection_new(Port);
  if(!ServerConn) {
    error(true, "Could not create server connection.");
    return 1;
  }

  Running = true;

  while(Running) {
    server_connection_handle_incoming(ServerConn);
  }

  return 0;
}
