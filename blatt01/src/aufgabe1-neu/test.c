#include <stdlib.h>
#include <stdio.h>
#include "list.h"

void print_elem(void *x)
{
  printf("%s\n", (char*)x);
}

int main(int argc, char **argv)
{
  list_t *list = list_new();
  int i;

  for(i = 0; i < 10; i++) {
    char *string = malloc(5);
    sprintf(string, "foo%d", i);
    list_insert(list, string);
  }

  list_foreach(list, &print_elem);

  printf("done\n");
  list_delete(list);
  return 0;
}
