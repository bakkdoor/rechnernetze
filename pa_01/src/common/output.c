#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "output.h"

void info(char * message, ...)
{
  va_list args;
  va_start(args, message);
  fprintf(stdout, "INFO: ");
  vfprintf(stdout, message, args);
  fprintf(stdout, "\n");
  va_end(args);
}

void warn(char * message, ...)
{
  va_list args;
  va_start(args, message);
  fprintf(stdout, "WARNING: ");
  vfprintf(stdout, message, args);
  fprintf(stdout, "\n");
  va_end(args);
}

void error(bool quit, char * message, ...)
{
  va_list args;
  va_start(args, message);
  fprintf(stderr, "ERROR: ");
  vfprintf(stderr, message, args);
  fprintf(stderr, "\n");
  va_end(args);
  if(quit)
    exit(1);
}
