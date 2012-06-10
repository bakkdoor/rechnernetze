#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdbool.h>
#include <stdarg.h>

void set_show_output_prefix(bool show_prefix);
void info(char * message, ...);
void warn(char * message, ...);
void error(bool quit, char * message, ...);

#endif /* _ERROR_H_ */
