#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

void error(char * message, bool quit)
{
  fprintf(stderr, "ERROR: %s\n", message);
  if(quit)
    exit(1);
}
