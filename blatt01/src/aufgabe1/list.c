#include <stdlib.h>
#include "list.h"

/*
  Private struct definitions
 */

typedef struct node {
  struct node *next;
  void *data;
} list_node_t;

struct list {
  list_node_t *first;
  size_t size;
};

/*
  Private functions
 */

list_node_t *node_new(void *data)
{
  list_node_t *node = (list_node_t*)malloc(sizeof(list_node_t));
  if(node) {
    node->next = NULL;
    node->data = data;
  }
  return node;
}


/*
  Public functions
 */

list_t *list_new(void)
{
  list_t *list = (list_t*)malloc(sizeof(list_t));
  if(list) {
    list->first = NULL;
    list->size = 0;
  }
  return list;
}


void list_delete(list_t *list, void(*delete_func)(void *data))
{
  list_clear(list, delete_func);
  free(list);
}

size_t list_size(list_t *list)
{
  if(list == NULL)
    return 0;

  return list->size;
}

void list_insert(list_t *list, void *data)
{
  if(list == NULL)
    return;

  list_node_t *current = list->first;

  if(list->first == NULL) {
    list->first = node_new(data);
    current = list->first;
  } else {
    // move to end
    while(current->next) {
      current = current->next;
    }
    current->next = node_new(data);
    current = current->next;
  }

  if(current)
    list->size++;
}

unsigned int list_insert_after(list_t *list, void *data, bool(*predicate)(void *a, void *b))
{
  if(list == NULL)
    return 0;

  if(list->first == NULL) {
    list_insert(list, data);
    return 0;
  }

  list_node_t *current = list->first;
  list_node_t *previous = NULL;
  list_node_t *new;
  unsigned int pos = 0;

  while(current) {
    if(previous) {
      if(predicate(previous->data, data)) {
        new = node_new(data);
        new->next = current;
        previous->next = new;
        list->size++;
        return pos;
      }
    } else {
      if(predicate(data, current->data)) {
        list_node_t *tmp = list->first;
        list->first = node_new(data);
        list->first->next = tmp;
        list->size++;
        return 1;
      }
    }

    previous = current;
    current = current->next;
    pos++;
  }

  previous->next = node_new(data);
  list->size++;

  return pos;
}

void list_remove(list_t *list, void *data, bool delete_all, int(*list_compare)(void *a, void *b), void(*delete_func)(void *data))  // defaults to comparing pointers
{
  if(list == NULL)
    return;

  list_node_t *current = list->first;
  list_node_t *tmp;
  while(current) {
    if(list_compare(current->data, data) == 0) {
      tmp = current;
      if(delete_all) {
        current = current->next;
      } else {
        current = NULL;
      }
      if(delete_func) {
        delete_func(tmp->data);
      }
      free(tmp);
      list->size--;
    }
  }
}

int compare_true(void *a, void *b)
{
  return 0;
}

void list_clear(list_t *list, void(*delete_func)(void *data))
{
  list_remove(list, 0, true, &compare_true, delete_func);
}


void list_foreach(list_t *list, void(*list_func)(void *a))
{
  if(list == NULL)
    return;

  list_node_t *current;
  for(current = list->first; current != NULL; current = current->next) {
    list_func(current->data);
  }
}

void list_sort(list_t *list, int(*list_compare)(void *a, void *b))
{
  /* TODO */
}

list_t *list_filter(list_t *list, bool(*predicate)(void *a))
{
  list_t *new = list_new();
  list_node_t *current = list->first;
  for(; current; current = current->next) {
    if(predicate(current->data)) {
      list_insert(new, current->data);
    }
  }
  return new;
}

list_t *list_map(list_t *list, void*(*func)(void *a))
{
  list_t *new = list_new();
  list_node_t *current = list->first;
  for(; current; current = current->next) {
    list_insert(new, func(current->data));
  }
  return new;
}
