#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdbool.h>
#include <stdarg.h>

/**
 * @param show_prefix If false disable prefix output (e.g. "INFO: ") in output functions
 **/
void set_show_output_prefix(bool show_prefix);

/**
 * @param fmt Format string (similar to printf()) - printed to stdout.
 **/
void info(char * fmt, ...);

/**
 * @param fmt Format string (similar to printf()) - printed to stdout.
 **/
void warn(char * fmt, ...);

/**
 * @param quit Quits the current process (via exit(1)) if true.
 * @param fmt Format string (similar to printf()) - printed to stderr.
 **/
void error(bool quit, char * fmt, ...);

#endif /* _ERROR_H_ */
