#include <stdio.h>
#include <stdlib.h>

void help(char * progname)
{
  puts("Usage:");
  printf("%s <server_addr> <server_port> <name>\n", progname);
}

int main(int argc, char ** argv)
{
  if(argc < 4) {
    help(argv[0]);
    return 1;
  }

  return 0;
}
