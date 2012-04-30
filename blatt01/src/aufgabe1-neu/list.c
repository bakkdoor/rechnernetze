#include <stdlib.h>
#include "list.h"

list_t *list_new(void)
{
  list_t *list = (list_t*)malloc(sizeof(list_t));
  printf("%d\n", list);
  if(list) {
    list->next = NULL;
    list->data = NULL;
  }
  return list;
}

void list_insert(list_t *list, void *data)
{
  list_t *node = list_new();
  if(node) {
    /* node->next = list->next; */
    node->data = data;
  }
  while(list->next != NULL) {
    list = list->next;
  }
  list->next = node;
}

void list_remove(list_t *list, void *data, bool delete_all, int(*list_compare)(void *a, void *b))  // defaults to comparing pointers
{
  list_t *tmp;
  int i = 0;
  while(list) {
    printf("%d\n", i);
    i++;
    if(list_compare(list->data, data) == 0) {
      tmp = list;
      list = list->next;
      free(tmp->data);
      printf("--->%d", tmp);
      printf("\n");
      free(tmp);
    }
  }
  /* free(list); */
}

int compare_true(void *a, void *b)
{
  return 0;
}

void list_clear(list_t *list)
{
  list_remove(list, 0, true, &compare_true);
}


void list_foreach(list_t *list, void(*list_func)(void *a))
{
  for(; list != NULL; list = list->next) {
    list_func(list->data);
  }
}

void list_delete(list_t *list)
{
  list_clear(list);
  free(list);
}
