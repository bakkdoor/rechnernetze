#ifndef _SERVER_MESSAGES_H_
#define _SERVER_MESSAGES_H_

typedef enum server_message {
  SV_CON_REP = 2,
  SV_ROOM_MSG = 5,
  SV_AMSG = 7,
  SV_DISC_REP = 9,
  SV_DISC_AMSG = 10
} server_message_e;

#endif /* _SERVER_MESSAGES_H_ */
