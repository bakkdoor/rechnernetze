#ifndef _LIST_H_
#define _LIST_H_

typedef struct node {
  struct node *next;
  void *data;
} list_node_t;

typedef struct list {
  list_node_t *first;
  unsigned int size;
} list_t;

typedef int bool;
#define true 1
#define false 0



list_t *list_new(void);
void list_insert(list_t *list, void *data);
void list_remove(list_t *list, void *data, bool delete_all, int(*list_compare)(void *a, void *b));
void list_clear(list_t *list);
void list_foreach(list_t *list, void(*list_func)(void *a));
void list_delete(list_t *list);

#endif /* _LIST_H_ */
