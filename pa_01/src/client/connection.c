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
#include "../common/unp_readline.h"

#define DEBUG

#define DEFAULT_TIMEOUT_SEC 5

#define JOIN_PRE_CMD "/join "
#define LEAVE_PRE_CMD "/leave "
#define DISCONNECT_CMD "/disconnect"

client_message_t * parse_client_message(const char * buf);

struct client_connection {
  int sock;
  struct addrinfo * server_addr_info;
  char * username;
};

client_connection_t * connection_setup(const char * server_hostname, const char * server_port, const char * username) {
  int count;
  client_connection_t * cli_conn;
  struct addrinfo hints;
  client_message_t * message;
  server_message_t * response;
  struct sockaddr_in * addr;


  cli_conn = calloc(1, sizeof (client_connection_t));
  if (!cli_conn) {
    //TODO
  }

  // resolve hostname to ip
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = 0; /* any */
  hints.ai_flags = 0;

  if (getaddrinfo(server_hostname, server_port, &hints, &cli_conn->server_addr_info) != 0) {
    error(true, "Could not resolve hostname '%s'\n", server_hostname);
  }

  cli_conn->server_addr_info->ai_canonname = calloc(strlen(server_hostname) + 1, sizeof (char));
  if (!cli_conn->server_addr_info->ai_canonname) {
    error(true, "Could not setup connection! Not enough memory?");
  }

  strcpy(cli_conn->server_addr_info->ai_canonname, server_hostname);

#ifdef DEBUG
  addr = (struct sockaddr_in *) cli_conn->server_addr_info->ai_addr;

  info("hostname: %s, ip: %s, port: %u",
          cli_conn->server_addr_info->ai_canonname,
          inet_ntoa(addr->sin_addr),
          ntohs(addr->sin_port));
#endif
  // end resolve hostname to ip

  // init user
  cli_conn->username = calloc(strlen(username) +1, sizeof(char));
  if (!cli_conn->username) {
    error(true, "Could not setup username! Not enough memory?");
  }
  strcpy(cli_conn->username, username);

  message = calloc(1, sizeof(client_message_t));
  if (!message) {
    error(true, "Could not create message! Not enough memory?");
  }

  message->type = CL_CON_REQ;
  message->cl_con_req.name = calloc(strlen(username) + 1, sizeof(char));
  if (!message->cl_con_req.name) {
    error(true, "Could not create message! Not enough memory?");
  }
  message->cl_con_req.length = strlen(username) + 1;
  strcpy(message->cl_con_req.name, username);

  cli_conn->sock = socket(cli_conn->server_addr_info->ai_family,
                          cli_conn->server_addr_info->ai_socktype,
                          cli_conn->server_addr_info->ai_protocol);
  
  if (cli_conn->sock < 0) {
    error(true, "Could not create socket!");
  }

  for (count = 0; count < 3; count++) {
    if (connection_send_client_message(cli_conn, message) > 0
            && connection_has_incoming_data(cli_conn, DEFAULT_TIMEOUT_SEC) > 0) {

      response = connection_recv_client_message(cli_conn);
      if (!response) {
        break;
      }

      if (response->type == SV_CON_REP) {
        if (response->sv_con_rep.state == CON_REP_OK) {
          addr = (struct sockaddr_in *) cli_conn->server_addr_info->ai_addr;

          addr->sin_port = ntohs(response->sv_con_rep.comm_port);
          cli_conn->server_addr_info->ai_addr = (struct sockaddr *)addr;

          info("Verbindung akzeptiert. Der Port für die weitere Kommunikation lautet %u.", ntohs(addr->sin_port));
          
          client_message_delete(message);
          server_message_delete(response);
          
          return cli_conn;
        } else {
          error(true, "Verbindung fehlgeschlagen. Benutzername %s bereits vergeben.", username);
        }
      }
    }
  }
  
  client_message_delete(message);
  if (response) server_message_delete(response);
  
  error(true, "Verbindung fehlgeschlagen. Wartezeit verstrichen.");
  return NULL;  // error terminate
}

void connection_close(client_connection_t * cli_conn) {
  int count;
  client_message_t * msg = parse_client_message(DISCONNECT_CMD);
  
  for (count = 3; count > 0; count --) {
    connection_send_client_message(cli_conn, msg);
    connection_handle_socks(cli_conn, DEFAULT_TIMEOUT_SEC);
  }
  
  connection_delete(cli_conn);
  error(true, "Verbindung nicht erfolgreich beendet. Wartezeit verstrichen.");
}

void connection_delete(client_connection_t * cli_conn) {
  free(cli_conn->username);
  freeaddrinfo(cli_conn->server_addr_info);
  free(cli_conn);
}

int connection_send_client_message(client_connection_t * cli_conn, client_message_t * msg) {
  char buff[MAX_CLIENT_MSG_SIZE];
  size_t length;

  memset(buff, 0, MAX_SERVER_MSG_SIZE);
  length  = client_message_write(msg, buff);

  return sendto(cli_conn->sock, buff, length, 0,
        (struct sockaddr *) cli_conn->server_addr_info->ai_addr,
        sizeof(struct sockaddr));
}

server_message_t * connection_recv_client_message(client_connection_t * cli_conn) {
  char buff[MAX_SERVER_MSG_SIZE];
  unsigned int slen = sizeof(struct sockaddr);

  memset(buff, 0, MAX_SERVER_MSG_SIZE);
  if (recvfrom(cli_conn->sock, buff, sizeof(buff), 0,
        (struct sockaddr *)cli_conn->server_addr_info->ai_addr, &slen) < 0) {

#ifdef DEBUG
    perror("recvfrom()");
#endif
    
    return NULL;
  }
  return server_message_read(buff);
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

void connection_handle_socks(client_connection_t * cli_conn, int timeout_sec) {
  fd_set read_fds;
  struct timeval timeout;
  server_message_t * srv_msg;
  client_message_t * cli_msg;
  char buff[MAX_CLIENT_MSG_SIZE];
  int size;
  
  memset(buff, 0, MAX_CLIENT_MSG_SIZE);

  if (!cli_conn->sock) return;

  timeout.tv_sec = timeout_sec;
  timeout.tv_usec = 0;

  FD_ZERO(&read_fds);
  FD_SET(cli_conn->sock, &read_fds);
  FD_SET(STDIN_FILENO, &read_fds);

  if (select(cli_conn->sock + 1, &read_fds, NULL, NULL, &timeout) > 0) {
    
    if (FD_ISSET(cli_conn->sock, &read_fds)) {
      srv_msg = connection_recv_client_message(cli_conn);
      handle_server_message(srv_msg);
      server_message_delete(srv_msg);
    } 
    if (FD_ISSET(STDIN_FILENO, &read_fds)) {
      readline(STDIN_FILENO, buff, MAX_CLIENT_MSG_SIZE);
      cli_msg = parse_client_message(buff);
      if (cli_msg != NULL) {
        if (cli_msg->type == CL_DISC_REQ) {
          connection_close(cli_conn);
        } else {
          size = connection_send_client_message(cli_conn, cli_msg);
          client_message_delete(cli_msg);
#ifdef DEBUG
          info ("send data: %d", size);
#endif
        }
      }
    }
  }
}

void handle_server_message(server_message_t * msg) {
  char format[MAX_SERVER_MSG_SIZE];
  memset(format, 0, MAX_SERVER_MSG_SIZE);
  
  switch(msg->type) {
    case SV_ROOM_MSG:
      switch(msg->sv_room_msg.action) {
        case SV_ROOM_MSG_ACTION_JOIN:
          sprintf(format, "%%%us hat den raum %%%us betreten.", msg->sv_room_msg.user_length, msg->sv_room_msg.room_length);
          info(format, msg->sv_room_msg.user, msg->sv_room_msg.room);
          break;
        case SV_ROOM_MSG_ACTION_LEAVE:
          sprintf(format, "%%%us hat den raum %%%us verlassen.", msg->sv_room_msg.user_length, msg->sv_room_msg.room_length);
          info(format, msg->sv_room_msg.user, msg->sv_room_msg.room);
          break;
        default: 
          assert(0);
      }
      break;
    case SV_AMSG:
      sprintf(format, "%%%us - %%%us: %%%us\n", msg->sv_amsg.room_length, msg->sv_amsg.user_length, msg->sv_amsg.msg_length);
      printf(format, msg->sv_amsg.room, msg->sv_amsg.user, msg->sv_amsg.msg);
      break;
    case SV_DISC_REP:
      info("Verbindung erfolgreich beendet.");
      exit(0);
      break;
    case SV_DISC_AMSG:
      sprintf(format, "%%%us hat das System verlassen.", msg->sv_disc_amsg.user_length);
      info(format, msg->sv_disc_amsg.user);
      break;
    default:
      assert(0);
  }
}

client_message_t * parse_client_message(const char * buf)
{
  client_message_t * message;

  message = calloc(1, sizeof(client_message_t));
  if (!message) return NULL;

  if (strncmp(JOIN_PRE_CMD, buf, strlen(JOIN_PRE_CMD)) == 0) {
    message->type = CL_ROOM_MSG;
    message->cl_room_msg.action = CL_ROOM_MSG_ACTION_JOIN;
    message->cl_room_msg.length = strlen(buf + strlen(JOIN_PRE_CMD)) + 1;
    message->cl_room_msg.room_name = calloc(message->cl_room_msg.length, sizeof(char));
    if (!message->cl_room_msg.room_name) {
      //TODO
    }
    if (!strcpy(message->cl_room_msg.room_name, buf + strlen(JOIN_PRE_CMD))) {
      //TODO
    }

  } else if (strncmp(LEAVE_PRE_CMD, buf, strlen(LEAVE_PRE_CMD)) == 0) {
    message->type = CL_ROOM_MSG;
    message->cl_room_msg.action = CL_ROOM_MSG_ACTION_LEAVE;
    message->cl_room_msg.length = strlen(buf + strlen(LEAVE_PRE_CMD)) + 1;
    message->cl_room_msg.room_name = calloc(message->cl_room_msg.length, sizeof(char));
    if (!message->cl_room_msg.room_name) {
      //TODO
    }
    if (!strcpy(message->cl_room_msg.room_name, buf + strlen(LEAVE_PRE_CMD))) {
      //TODO
    }

  } else if (strncmp(DISCONNECT_CMD, buf, strlen(DISCONNECT_CMD)) == 0) {
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
      client_message_delete(message);
      return NULL;
    }
  }

  return message;
}
