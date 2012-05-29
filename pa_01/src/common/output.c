#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "output.h"

void error(char * message, bool quit)
{
  fprintf(stderr, "ERROR: %s\n", message);
  if(quit)
    exit(1);
}
