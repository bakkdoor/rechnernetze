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
  struct sockaddr_in * server_addr;
  int incoming_sock;
  struct sockaddr_in * incoming_addr;
};

client_connection_t * connection_setup(const char * server_hostname, int server_port, char * username)
{
  int count;
  client_connection_t * cli_conn;
  client_message_t message;
  server_message_t * reply_msg;
  hostent_t * he;

  cli_conn = calloc(1, sizeof(client_connection_t));
  if (!cli_conn) {
    error(true, "Could not allocate memory!");
  }

  cli_conn->sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (cli_conn->sock < 0) {
    error(true, "Could not create socket!");
  }

  cli_conn->server_addr = calloc(1, sizeof(struct sockaddr_in));
  if (!cli_conn->server_addr) {
    close(cli_conn->sock);
    error(true, "Could not setup server address!");
  }

  if (inet_addr(server_hostname) == -1) {
    he = get_host_ip4(server_hostname);
    if (he != NULL && he->h_addr_list != NULL) {
      cli_conn->server_addr->sin_addr.s_addr = inet_addr(he->h_addr_list[0]);
    } else {
      error(true, "Bad server hostname!");
    }
  } else {
    cli_conn->server_addr->sin_addr.s_addr = inet_addr(server_hostname);
  }

  cli_conn->server_addr->sin_family = AF_INET;
  cli_conn->server_addr->sin_port = htons(server_port);
  cli_conn->username = username;

  message.type = CL_CON_REQ;
  message.cl_con_req.length = strlen(username) + 1;
  // TODO
  message.cl_con_req.name = username;

  for (count = 0; count < 3; count++) {

    connection_send_client_message(cli_conn, &message);
    if (connection_has_incoming_data(cli_conn->sock, DEFAULT_TIMEOUT_SEC) > 0) {
      reply_msg = connection_recv_client_message(cli_conn);
      if (reply_msg) {
        if (reply_msg->type == SV_CON_REP) {
          if (reply_msg->sv_con_rep.state == CON_REP_OK) {
            info("Verbindung akzeptiert. Der Port für die weitere Kommunikation lautet %u.",
                   reply_msg->sv_con_rep.comm_port);

            //cli_conn->server_addr->sin_port = htons(reply_msg->sv_con_rep.comm_port);
            cli_conn->incoming_addr->sin_family = AF_INET;
            cli_conn->incoming_addr->sin_addr.s_addr = htonl(INADDR_ANY);
            cli_conn->incoming_addr->sin_port = htons(reply_msg->sv_con_rep.comm_port);

            cli_conn->incoming_sock = socket(AF_INET, SOCK_DGRAM, 0);
            if (!cli_conn->incoming_sock) {
              error(true, "Could not create incoming socket!");
            }

            if (bind(cli_conn->incoming_sock, (struct sockaddr *) cli_conn->incoming_addr, sizeof(cli_conn->incoming_addr)) < 0) {
              // can not bind socket at this port!!!
              // TODO
            }

          } else if (reply_msg->sv_con_rep.state == CON_REP_BAD_USERNAME) {
            info("Verbindung fehlgeschlagen. Benutzername %s bereits vergeben.", username);
          }
        }
        free(reply_msg);
        return cli_conn;
      }
    }
  }
  error(false, "Verbindung fehlgeschlagen. Wartezeit verstrichen.");

  close(cli_conn->sock);
  /* ???
  free(cli_conn->username);
  free(cli_conn->server_addr);
  */
  free(cli_conn);
  return NULL;
}

int connection_close(client_connection_t * cli_conn)
{
  int count;
  client_message_t msg;
  server_message_t * reply_msg;
  hostent_t *he;

  msg.type = CL_DISC_REQ;

  he = get_host_name(&cli_conn->server_addr->sin_addr);
  if (!he) {
    he = get_host_ip4(inet_ntoa(cli_conn->server_addr->sin_addr));
    if (!he) {
      assert(false);
    }
  }

  info("Beende die Verbindung zu Server %s (%s)",
       he->h_name, inet_ntoa(cli_conn->server_addr->sin_addr));

  for (count = 0; count < 3; count++) {
    // try to send DISC_REQ
    if (connection_send_client_message(cli_conn, &msg) > 0) {
      // data send, parse incoming...

      while(connection_has_incoming_data(cli_conn->incoming_sock, DEFAULT_TIMEOUT_SEC) > 0) {

        reply_msg = connection_recv_client_message(cli_conn);
        if (reply_msg) {
          if (reply_msg->type == SV_DISC_REP) {
            info("Verbindung erfolgreich beendet.");
            break;
          } else {
            // TODO handle message
          }
          free(reply_msg);
        }
      }
    }
  }

  if (count == 3) {
    info("Verbindung nicht erfolgreich beendet. Wartezeit verstrichen.");
  }

  if (close(cli_conn->sock) == 0 && close(cli_conn->incoming_sock) == 0) {
    free(cli_conn);
    return 0;
  } else {
    return -1;
  }
}

int connection_send_client_message(client_connection_t * cli_conn, client_message_t * msg)
{
  int bytes_send;
  char * buf = calloc(MAX_CLIENT_MSG_SIZE, sizeof(char));
  size_t len = client_message_write(msg, buf);

  //bytes_send = write(sock, buf, len);

  bytes_send = sendto(cli_conn->sock, buf, len, 0,
                      (struct sockaddr *) cli_conn->server_addr,
                      sizeof(struct sockaddr_in));

  free(buf); /* ?!?! TODO: Check if OK here. */

  return bytes_send;
}

server_message_t * connection_recv_client_message(client_connection_t * cli_conn)
{
  char * buf;
  /* size_t len; */
  int bytes_read;
  server_message_t * incoming_message = NULL;

  buf = calloc(MAX_SERVER_MSG_SIZE, sizeof(char));

  bytes_read = recvfrom(cli_conn->incoming_sock, buf, sizeof(buf), 0,
                        (struct sockaddr *) cli_conn->incoming_addr,
                        (socklen_t *) sizeof(struct sockaddr_in));

  if (bytes_read < 1) {
    free(buf);
    return NULL;
  }

  incoming_message = server_message_read(buf);

  free(buf);
  return incoming_message;
}

int connection_has_incoming_data(int sockfd, int timeout_sec) {
  fd_set read_fds;
  struct timeval timeout;

  if (!sockfd) return 0;

  timeout.tv_sec = timeout_sec;
  timeout.tv_usec = 0;

  FD_ZERO(&read_fds);
  FD_SET(sockfd, &read_fds);

  return select(sockfd + 1, &read_fds, NULL, NULL, &timeout);
}
