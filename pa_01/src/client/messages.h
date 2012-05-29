#ifndef _CLIENT_MESSAGES_H_
#define _CLIENT_MESSAGES_H_

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
  int length : 4;
  char * name;
} msg_cl_con_req_t;

typedef struct msg_cl_room_msg {
  int length : 4;
  char * room_name;
  cl_room_msg_action_e action;
} msg_cl_room_msg_t;

typedef struct msg_cl_msg {
  int room_length : 4;
  char * room_name;
  int msg_length : 4;
  char * message;
} msg_cl_msg_t;

typedef struct cl_disc_req {
} msg_cl_disc_req_t;

typedef struct client_message {
  client_message_e type : CL_CON_REQ;
  union {
    msg_cl_con_req_t cl_con_req;
    msg_cl_room_msg_t cl_room_msg;
    msg_cl_msg_t cl_msg;
    msg_cl_disc_req_t cl_disc_req; 
  };
} client_message_t;

#endif /* _CLIENT_MESSAGES_H_ */
