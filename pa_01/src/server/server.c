#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../common/list.h"
#include "../common/chat_room.h"
#include "../common/chat_user.h"

#include "messages.h"
#include "connection.h"

static list_t * chat_rooms;
static list_t * chat_users;

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
  chat_users = list_new();

  printf("Chat Server - Listening on %d\n", Port);

  ServerConn = server_connection_new(Port);
  if(!ServerConn) {
    fprintf(stderr, "Fail :(\n");
    return 1;
  }

  Running = true;

  while(Running) {
    server_connection_handle_incoming(ServerConn);
  }

  return 0;
}
