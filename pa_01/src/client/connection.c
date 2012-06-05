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

#include "connection.h"
#include "../common/output.h"
#include "../common/chat_user.h"

#define DEBUG

enum {
  DEFAULT_TIMEOUT_SEC = 5,
  
};

#define JOIN_PRE_CMD "/join "
#define LEAVE_PRE_CMD "/leave "
#define DISCONNECT_CMD "/disconnect"

struct client_connection {
  int sock;
  struct addrinfo * server_addr_info;
  const char * username;
};

client_connection_t * connection_setup(const char * server_hostname, const char * server_port, char * username) {
  client_connection_t * cli_conn;
  struct addrinfo * hints;
  struct sockaddr_in * addr;

  cli_conn = calloc(1, sizeof (client_connection_t));
  if (!cli_conn) {
    //TODO
  }

  // resolve hostname to ip
  hints = calloc(1, sizeof (struct addrinfo));
  if (!hints) {
    //TODO
  }

  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_DGRAM;
  hints->ai_protocol = 0; /* any */
  hints->ai_flags = 0;

  if (getaddrinfo(server_hostname, server_port, hints, &cli_conn->server_addr_info) != 0) {
    error(false, "Could not resolve hostname '%s'\n", server_hostname);
    //TODO free mem
  }

  cli_conn->server_addr_info->ai_canonname = calloc(strlen(server_hostname) + 1, sizeof (char));
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
  // end resolve hostname to ip

  // init user
  cli_conn->username = calloc(strlen(username) +1, sizeof(char));
  if (!cli_conn->username) {
    //TODO
  }
  strcpy(cli_conn->username, username);


  // TODO create sock
  // TODO send server request

  return cli_conn;
}

int connection_close(client_connection_t * cli_conn) {
  return 0;
}

void connection_delete(client_connection_t * cli_conn) {
  free(cli_conn->username);
  freeaddrinfo(cli_conn->server_addr_info);

  free(cli_conn);
}

int connection_send_client_message(client_connection_t * cli_conn, client_message_t * msg) {
  return 0;
}

server_message_t * connection_recv_client_message(client_connection_t * cli_conn, bool incoming) {
  return NULL;
}

int connection_has_incoming_data(int sockfd, int timeout_sec) {
  return 0;
}

client_message_t * parse_client_message(const char * buf) 
{
  client_message_t * message;
  
  message = calloc(1, sizeof(client_message_t));
  if (!message) return NULL;
    
  if (strncmp(JOIN_PRE_CMD, buf, sizeof(JOIN_PRE_CMD)) == 0) {
    message->type = CL_ROOM_MSG;
    message->cl_room_msg.action = CL_ROOM_MSG_ACTION_JOIN;
    message->cl_room_msg.room_name = calloc(strlen(buf + strlen(JOIN_PRE_CMD)) + 1, sizeof(char));
    if (!message->cl_room_msg.room_name) {
      //TODO
    }
    if (!strcpy(message->cl_room_msg.room_name, buf + strlen(JOIN_PRE_CMD))) {
      //TODO
    }
    message->cl_room_msg.length = strlen(message->cl_room_msg.room_name) +1;
    
  } else if (strncmp(LEAVE_PRE_CMD, buf, sizeof(LEAVE_PRE_CMD)) == 0) {
    message->type = CL_ROOM_MSG_ACTION_LEAVE;
    message->cl_room_msg.action = CL_ROOM_MSG_ACTION_LEAVE;
    message->cl_room_msg.room_name = calloc(strlen(buf + strlen(LEAVE_PRE_CMD)) + 1, sizeof(char));
    if (!message->cl_room_msg.room_name) {
      //TODO
    }
    if (!strcpy(message->cl_room_msg.room_name, buf + strlen(LEAVE_PRE_CMD))) {
      //TODO
    }
    message->cl_room_msg.length = strlen(message->cl_room_msg.room_name) +1;
    
  } else if (strncmp(DISCONNECT_CMD, buf, sizeof(DISCONNECT_CMD)) == 0) {
    message->type = CL_DISC_REQ;
    
  } else {
    message->type = CL_MSG;
    
    if (strchr(buf, " ") > buf // contains a blank char
            && strchr(buf, " ") < buf + strlen(buf)) { // has chars after blank
      message->cl_msg.room_length = strchr(buf, " ") - buf + 1;
      message->cl_msg.room_name = calloc(message->cl_msg.room_length, sizeof(char));
      if (!message->cl_msg.room_name) {
        //TODO
      }
      
      strncpy(message->cl_msg.room_name, buf, message->cl_msg.room_length -1);
      
      message->cl_msg.msg_length = strlen(buf + message->cl_msg.room_length) +1;
      message->cl_msg.message = calloc(message->cl_msg.msg_length, sizeof(char));
      if (!message->cl_msg.message) {
        //TODO
      }
      
      strcpy(message->cl_msg.message, buf + message->cl_msg.room_length);
      
    } else {
      // TODO
      return NULL;
    }
  }
  
}