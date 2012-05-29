#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>


#include "connection.h"
#include "../common/list.h"
#include "../common/output.h"

struct server_connection {
  int sock;
  int port;
  struct sockaddr_in * addr;
  list_t  * client_connections;
};

struct client_connection {
  int sock;
  struct sockaddr_in * addr;
  list_t * rooms;
};

typedef struct client_connection client_connection_t;

client_connection_t * create_client_connection(struct sockaddr_in * addr);
bool client_in_room(const client_connection_t * cli_conn, const char * room);
void close_and_delete_client_connection(client_connection_t * cli_conn);

struct room {
  char * name;
};

typedef struct room room_t;

void delete_room(room_t * room);


////////////////////////////////////////////////////////

server_connection_t * server_connection_new(int port)
{
  int sockfd, err;
  struct sockaddr_in * addr;

  server_connection_t * server_conn = calloc(1, sizeof(server_connection_t));
  if(!server_conn)
    return NULL;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0) {
    error(true, "Could not setup server socket");
  }

  server_conn->sock = sockfd;
  server_conn->port = port;

  addr = calloc(1, sizeof(struct sockaddr_in));
  if(!addr) {
    error(true, "Could not setup sockaddr_in struct");
  }

  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  addr->sin_addr.s_addr = htonl(INADDR_ANY);

  err = bind(sockfd, (struct sockaddr *) addr, sizeof(struct sockaddr_in));
  if(err < 0) {
    error(true, "Could not bind on port");
  }

  server_conn->addr = addr;

  return server_conn;
}

void server_connection_delete(server_connection_t * server_conn)
{
  if(!server_conn)
    return;

  free(server_conn->addr);
  free(server_conn);
}

bool connection_close(server_connection_t * server_conn)
{
  if(!server_conn)
    return false;
  
  list_foreach(server_conn->client_connections, close_and_delete_client_connection);
  
  return close(server_conn->sock);
}

void server_connection_handle_incoming(server_connection_t * server_conn)
{
    struct timeval timeout;
    
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    
    select(0, NULL, NULL, NULL, &timeout);
  /* TODO:
     - recvfrom(server_conn->sock);
     - dispatch messages to connected clients on same chat_room
     - ?
   */
}


client_connection_t * create_client_connection(struct sockaddr_in * addr) {
  client_connection_t * cli_conn;
  
  cli_conn = calloc(1, sizeof(client_connection_t));
  if (!cli_conn) { 
    //TODO
  }
  
  cli_conn->addr->sin_family = AF_INET;
  cli_conn->addr->sin_port = 0;
  cli_conn->addr->sin_addr.s_addr = addr->sin_addr.s_addr;
  
  cli_conn->sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (cli_conn->sock < 0) {
    //TODO
  }
  
  if (bind(cli_conn->sock, (struct sockaddr *)cli_conn->addr, sizeof(struct sockaddr_in)) < 0) {
    // TODO
  }
  
  if (getsockname(cli_conn->sock, (struct sockaddr *)cli_conn->addr, sizeof(struct sockaddr_in)) < 0) {
    // TODO
  };
  
  return cli_conn;
}

void delete_room(room_t * room) {
  free(room->name);
  free(room);
}

// list function to delete client connection
void close_and_delete_client_connection(client_connection_t * cli_conn) {
  
  if (close(cli_conn->sock) < 0) {
    //TODO
  }
  
  list_clear(cli_conn->rooms, delete_room);
  free(cli_conn->rooms);
  free(cli_conn->addr);
  free(cli_conn);
}


bool client_in_room(const client_connection_t * cli_conn, const char * room) {
  //TODO !!!!!!!!
  return false;
}
