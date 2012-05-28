#ifndef _SERVER_MESSAGES_H_
#define _SERVER_MESSAGES_H_

#define MAX_SERVER_MSG_SIZE 1500

typedef enum {
  SV_CON_REP = 2,
  SV_ROOM_MSG = 5,
  SV_AMSG = 7,
  SV_DISC_REP = 9,
  SV_DISC_AMSG = 10
} server_message_e;

typedef enum result_int_value {
  CON_REP_OK = 0, // connection accept
  CON_REP_BAD_USERNAME = 1 // connection refused (username exist)
} result_int_value_e;

typedef struct msg_sv_con_rep {
  result_int_value_e result; 	// CON_REP_OK || CON_REP_BAD_USERNAME
  int comm_port;
} msg_sv_con_rep_t;


typedef struct server_message {
  server_message_e type : SV_CON_REP;
  union {
    msg_sv_con_rep_t sv_con_rep;
  };
} server_message_t;

void server_message_delete(server_message_t * server_message);

#endif /* _SERVER_MESSAGES_H_ */
