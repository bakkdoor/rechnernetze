#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "network_conversations.h"

char * str_to_net(const char * str) {
  char * net_str;
  int i;
  
  net_str = calloc(strlen(str)+1, sizeof(char));
  if (!net_str) return NULL;
  
  for (i = 0; i < strlen(str); i++) {
    net_str[i] = htons(str[i]);
  }
  
  net_str[i] = htons('\0');
  return net_str;
}

char * net_to_str(const char * str) {
  char * c_str;
  int i;
  
  c_str = calloc(strlen(str)+1, sizeof(char));
  if (!c_str) return NULL;
  
  for (i = 0; i < strlen(str); i++) {
    c_str[i] = ntohs(str[i]);
  }
  
  c_str[i] = ntohs('\0');
  return c_str;
}