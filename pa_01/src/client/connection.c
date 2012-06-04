#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/select.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <bits/socket.h>

#include "connection.h"
#include "../common/output.h"
#include "../common/dnsutils.h"
#include "../common/network_conversations.h"
#include "../common/messages.h"

enum {
  DEFAULT_TIMEOUT_SEC = 5
};

struct client_connection {
  char * username;
  int sock;
  struct addrinfo * server_addr_info;
};

client_connection_t * connection_setup(const char * server_hostname, const char * server_port, char * username) {
  client_connection_t * cli_conn;
  struct addrinfo  * hints;
  struct sockaddr_in * addr;
  
  cli_conn = calloc(1, sizeof(client_connection_t));
  if (!cli_conn) {
    //TODO
  }
  
  hints = calloc(1, sizeof(struct addrinfo));
  if (!hints) {
    //TODO
  }
  
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_DGRAM;
  hints->ai_protocol = 0; /* any */
  hints->ai_flags = 0;
  
  if (getaddrinfo(server_hostname, server_port, hints, &cli_conn->server_addr_info) != 0) {
    error(true, "Could not resolve hostname '%s'\n", server_hostname);
  }
  
  cli_conn->server_addr_info->ai_canonname = calloc(strlen(server_hostname) +1, sizeof(char));
  if (!cli_conn->server_addr_info->ai_canonname) {
    //TODO
  }
  
  strcpy(cli_conn->server_addr_info->ai_canonname, server_hostname);
  
#ifdef DEBUG
  addr = cli_conn->server_addr_info->ai_addr;
  
  printf("hostname: %s, ip: %s, port: %u\n", 
    cli_conn->server_addr_info->ai_canonname,
    inet_ntoa(addr->sin_addr),
    ntohs(addr->sin_port));
#endif
  
  // TODO create sock
  // TODO send server request
  
  return cli_conn;
}

int connection_close(client_connection_t * cli_conn)
{
  return 0;
}

int connection_send_client_message(client_connection_t * cli_conn, client_message_t * msg)
{
  return 0;
}

server_message_t * connection_recv_client_message(client_connection_t * cli_conn, bool incoming)
{
  return NULL;
}

int connection_has_incoming_data(int sockfd, int timeout_sec) 
{
  return 0;
}
