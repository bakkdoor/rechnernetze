#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdbool.h>
#include <stdarg.h>

void info(char * message, ...);
void warn(char * message, ...);
void error(bool quit, char * message, ...);

#endif /* _ERROR_H_ */
