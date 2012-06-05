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
  char * username;
};

client_connection_t * connection_setup(const char * server_hostname, const char * server_port, const char * username) {
  int count;
  client_connection_t * cli_conn;
  struct addrinfo * hints;
  client_message_t * message;
  server_message_t * response;
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
  addr = (struct sockaddr_in *) cli_conn->server_addr_info->ai_addr;

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

  message = calloc(1, sizeof(client_message_t));
  if (!message) {
    //TODO
  }
  
  message->type = CL_CON_REQ;
  message->cl_con_req.name = calloc(strlen(username) + 1, sizeof(char));
  if (!message->cl_con_req.name) {
    //TODO
  }
  message->cl_con_req.length = strlen(username) + 1;
  strcpy(message->cl_con_req.name, username);
  
  SOCK_DGRAM;
  cli_conn->sock = socket(cli_conn->server_addr_info->ai_family, 
                          cli_conn->server_addr_info->ai_socktype, 
                          cli_conn->server_addr_info->ai_protocol);
  if (cli_conn->sock < 0) {
    error(true, "Could not create socket!");
  }
  
  for (count = 0; count < 3; count++) {
    if (connection_send_client_message(cli_conn, message) > 0 
            && connection_has_incoming_data(cli_conn, DEFAULT_TIMEOUT_SEC)) {
      
      response = connection_recv_client_message(cli_conn);
      if (!response) {
        break;
      }
      
      if (response->type == SV_CON_REP) {
        if (response->sv_con_rep.state == CON_REP_OK) {
          addr = (struct sockaddr_in *) cli_conn->server_addr_info->ai_addr;
          addr->sin_port = response->sv_con_rep.comm_port;
          cli_conn->server_addr_info->ai_addr = addr;
          
          info("Verbindung akzeptiert. Der Port für die weitere Kommunikation lautet %u.", ntohs(addr->sin_port));
          
          return cli_conn;
        } else {
          error(true, "Verbindung fehlgeschlagen. Benutzername %s bereits vergeben.", username);
        }
      }
    }
  }
  
  error(true, "Verbindung fehlgeschlagen. Wartezeit verstrichen.");
  return NULL;  // error terminate
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
  char buff[MAX_CLIENT_MSG_SIZE];
  size_t length  = client_message_write(msg, buff);
  
  return sendto(cli_conn->sock, buff, length, 0, 
        (struct sockaddr *) cli_conn->server_addr_info->ai_addr, 
        sizeof(struct sockaddr));
}

server_message_t * connection_recv_client_message(client_connection_t * cli_conn) {
  char buff[MAX_SERVER_MSG_SIZE];
  
  if (recvfrom(cli_conn->sock, buff, sizeof(buff), 0,  
        (struct sockaddr *)cli_conn->server_addr_info->ai_addr, sizeof(struct sockaddr)) > 0) {
    return server_message_read(buff);
  }
  return NULL;
}

int connection_has_incoming_data(client_connection_t * cli_conn, int timeout_sec) {
  fd_set read_fds;
  struct timeval timeout;

  if (!cli_conn->sock) return 0;

  timeout.tv_sec = timeout_sec;
  timeout.tv_usec = 0;

  FD_ZERO(&read_fds);
  FD_SET(cli_conn->sock, &read_fds);

  return select(cli_conn->sock + 1, &read_fds, NULL, NULL, &timeout);
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

    if (strchr(buf, ' ') > buf // contains a blank char
            && strchr(buf, ' ') < buf + strlen(buf)) { // has chars after blank
      message->cl_msg.room_length = strchr(buf, ' ') - buf + 1;
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

  return message;
}
