#ifndef _LIST_H_
#define _LIST_H_

#include "bool.h"

/*
 Public List API
*/

struct list;
typedef struct list list_t;

list_t *list_new(void);
void list_delete(list_t *list);
size_t list_size(list_t *list);
void list_insert(list_t *list, void *data);
void list_remove(list_t *list, void *data, bool delete_all, int(*list_compare)(void *a, void *b));
void list_clear(list_t *list);
void list_foreach(list_t *list, void(*list_func)(void *a));
void list_sort(list_t *list, int(*list_compare)(void *a, void *b));

#endif /* _LIST_H_ */
