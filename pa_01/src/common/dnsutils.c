#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "dnsutils.h"

hostent_t * get_host_name_str(const char * ip4_addr) {
  struct hostent *he;
  struct in_addr ipv4addr;

  inet_pton(AF_INET, ip4_addr, &ipv4addr);
  he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
  return he;
}

hostent_t * get_host_name(const struct in_addr * ip4_addr) {
  struct hostent *he;

  he = gethostbyaddr(ip4_addr, sizeof ip4_addr, AF_INET);
  return he;
}

hostent_t * get_host_ip4(const char * hostname) {
  struct hostent * he;
    
  he = gethostbyname(hostname);
  if (he == NULL) return NULL;
  
  return he;
}
