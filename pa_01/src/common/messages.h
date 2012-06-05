#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include <stdint.h>

/*
   Client Messages
*/

#define MAX_CLIENT_MSG_SIZE 1500

typedef enum {
  CL_CON_REQ = 1,
  CL_ROOM_MSG = 4,
  CL_MSG = 6,
  CL_DISC_REQ = 8
} client_message_e;

typedef enum {
  CL_ROOM_MSG_ACTION_JOIN = 0,
  CL_ROOM_MSG_ACTION_LEAVE = 1
} cl_room_msg_action_e;


typedef struct msg_cl_con_req {
  uint32_t length;
  char * name;
} msg_cl_con_req_t;

typedef struct msg_cl_room_msg {
  uint32_t length;
  char * room_name;
  uint8_t action;
} msg_cl_room_msg_t;

typedef struct msg_cl_msg {
  uint32_t room_length;
  char * room_name;
  uint32_t msg_length;
  char * message;
} msg_cl_msg_t;

typedef struct cl_disc_req {
} msg_cl_disc_req_t;

typedef struct client_message {
  uint8_t type;
  union {
    msg_cl_con_req_t cl_con_req;
    msg_cl_room_msg_t cl_room_msg;
    msg_cl_msg_t cl_msg;
    msg_cl_disc_req_t cl_disc_req;
  };
} client_message_t;

void client_message_delete(client_message_t * client_message);
void _client_message_delete(void * client_message);
client_message_t * client_message_read(char * buf);
size_t client_message_write(client_message_t * client_message, char * buf);


/*
   Server Messages
*/


#define MAX_SERVER_MSG_SIZE 1500

typedef enum {
  SV_CON_REP = 2,
  SV_ROOM_MSG = 5,
  SV_AMSG = 7,
  SV_DISC_REP = 9,
  SV_DISC_AMSG = 10
} server_message_e;

typedef enum {
  CON_REP_OK = 0, // connection accept
  CON_REP_BAD_USERNAME = 1 // connection refused (username exist)
} con_rep_state_e;

typedef enum {
  SV_ROOM_MSG_ACTION_JOIN = 0,
  SV_ROOM_MSG_ACTION_LEAVE = 1,
} sv_room_msg_action_e;


typedef struct sv_con_rep {
  uint8_t state; 	// CON_REP_OK || CON_REP_BAD_USERNAME
  uint32_t comm_port;
} msg_sv_con_rep_t;

typedef struct sv_room_msg {
  uint32_t room_length;
  char * room;
  uint32_t user_length;
  char * user;
  uint8_t action;
} msg_sv_room_msg_t;

typedef struct sv_amsg {
  uint32_t room_length;
  char * room;
  uint32_t user_length;
  char * user;
  uint32_t msg_length;
  char * msg;
} msg_sv_amsg_t;

typedef struct sv_disc_rep {
} msg_sv_disc_rep_t;

typedef struct sv_disc_amsg {
  uint32_t user_length;
  char * user;
} msg_ssv_disc_amsg_t;


typedef struct server_message {
  uint8_t type;
  union {
    msg_sv_con_rep_t sv_con_rep;
    msg_sv_room_msg_t sv_room_msg;
    msg_sv_amsg_t sv_amsg;
    msg_sv_disc_rep_t sv_disc_rep;
    msg_ssv_disc_amsg_t sv_disc_amsg;
  };
} server_message_t;

void server_message_delete(server_message_t * server_message);
void _server_message_delete(void * server_message);
server_message_t * server_message_read(char * buf);
size_t server_message_write(server_message_t * server_message, char * buf);

#endif /* _MESSAGES_H_ */
