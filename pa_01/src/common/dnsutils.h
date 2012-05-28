#ifndef _DNSUTILS_H_
#define _DNSUTILS_H_

#include <netdb.h>

typedef struct hostent hostent_t;

hostent_t * get_host_name_str(const char * ip4_addr);
hostent_t * get_host_name(const struct in_addr * ip4_addr);
hostent_t * get_host_ip4(const char * hostname);

#endif /* _DNS_UTILS_H_ */