#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "output.h"

static bool _output_show_prefix = true;

void set_show_output_prefix(bool show_prefix)
{
  _output_show_prefix = show_prefix;
}

void info(char * fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  if(_output_show_prefix)
    fprintf(stdout, "INFO: ");
  vfprintf(stdout, fmt, args);
  fprintf(stdout, "\n");
  va_end(args);
}

void warn(char * fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  if(_output_show_prefix)
    fprintf(stdout, "WARNING: ");
  vfprintf(stdout, fmt, args);
  fprintf(stdout, "\n");
  va_end(args);
}

void error(bool quit, char * fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  if(_output_show_prefix)
    fprintf(stderr, "ERROR: ");
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);
  if(quit)
    exit(1);
}
