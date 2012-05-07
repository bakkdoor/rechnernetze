#include <stdio.h>
#include <stdlib.h>

#include "common/list.h"
#include "common/chat_room.h"
#include "common/chat_user.h"

#include "messages.h"
#include "connection.h"

static list_t * chat_rooms;
static list_t * chat_users;

void help(char * progname)
{
  puts("Usage:");
  printf("%s <server_port>\n", progname);
}

int main(int argc, char ** argv)
{
  if(argc == 1) {
    help(argv[0]);
    return 1;
  }

  int port = atoi(argv[1]);
  int sockfd;

  chat_rooms = list_new();
  chat_users = list_new();

  printf("Chat Server - Listening on %d\n", port);

  sockfd = connection_setup(port);
  if(!sockfd) {
    fprintf(stderr, "Fail :(\n");
    return 1;
  }

  return 0;
}
