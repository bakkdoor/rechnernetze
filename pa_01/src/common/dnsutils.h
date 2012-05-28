#ifndef _DNSUTILS_H_
#define _DNSUTILS_H_

#include <netdb.h>

typedef struct hostent hostent_t;

hostent_t * get_host_name_str(char * ip4_addr);
hostent_t * get_host_name(struct in_addr * ip4_addr);
hostent_t * get_host_ip4(char * hostname);

#endif /* _DNS_UTILS_H_ */