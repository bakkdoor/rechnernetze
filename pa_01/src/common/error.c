#include <stdio.h>
#include <stdlib.h>

#include "error.h"

void error(char * message, bool quit)
{
  fprintf(stderr, "%s\n", message);
  if(quit)
    exit(1);
}
