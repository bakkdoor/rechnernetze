#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "output.h"

void error(bool quit, char * message)
{
  fprintf(stderr, "ERROR: %s\n", message);
  if(quit)
    exit(1);
}
