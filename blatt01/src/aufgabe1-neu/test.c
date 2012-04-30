#include <stdlib.h>
#include <stdio.h>
#include "list.h"

void print_elem(void *x)
{
  printf("in printelem: ");
  printf("%s\n", (char*)x);
}

int main(int argc, char **argv)
{
  printf("woot\n");
  list_t *list = list_new();
  int i;

  for(i = 0; i < 10; i++) {
    printf("%d ", i);
    char *string = malloc(4);
    sprintf(string, "foo%d", i);
    list_insert(list, (void*)string);
  }

  list_foreach(list, &print_elem);

  printf("done\n");
  list_delete(list);
  return 0;
}
