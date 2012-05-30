#ifndef _SERVER_CLIENT_H
#define _SERVER_CLIENT_H

#include <arpa/inet.h>

#include "../common/chat_user.h"

typedef struct client client_t;

client_t * client_new(chat_user_t * char_user, struct sockaddr_in * client_addr);
void client_delete(client_t * client);
bool client_in_room(const client_t * client, const char * room_name);

#endif /* _SERVER_CLIENT_H */
