/*

This code has been taken from the 3rd edition of "UNIX Network Programming",
Volume1 by W. Richard Stevens, Bill Fenner, and Andrew M. Rudoff

See http://www.unpbook.com

*/


#ifndef UNP_READLINE_H
#define UNP_READLINE_H

#include <unistd.h>

#define MAXLINE         4096    /* max text line length */

ssize_t readline(int fd, void *vptr, size_t maxlen);


#endif /* UNP_READLINE_H */
