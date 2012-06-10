#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "output.h"

static bool _output_show_prefix = true;

void set_show_output_prefix(bool show_prefix)
{
  _output_show_prefix = show_prefix;
}

void info(char * message, ...)
{
  va_list args;
  va_start(args, message);
  if(_output_show_prefix)
    fprintf(stdout, "INFO: ");
  vfprintf(stdout, message, args);
  fprintf(stdout, "\n");
  va_end(args);
}

void warn(char * message, ...)
{
  va_list args;
  va_start(args, message);
  if(_output_show_prefix)
    fprintf(stdout, "WARNING: ");
  vfprintf(stdout, message, args);
  fprintf(stdout, "\n");
  va_end(args);
}

void error(bool quit, char * message, ...)
{
  va_list args;
  va_start(args, message);
  if(_output_show_prefix)
    fprintf(stderr, "ERROR: ");
  vfprintf(stderr, message, args);
  fprintf(stderr, "\n");
  va_end(args);
  if(quit)
    exit(1);
}
