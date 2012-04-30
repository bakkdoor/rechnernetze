#ifndef _LIST_H_
#define _LIST_H_

struct list;
typedef struct list list_t;

typedef int bool;
#define true 1
#define false 0


/*
 Public List API
*/

list_t *list_new(void);
void list_insert(list_t *list, void *data);
void list_remove(list_t *list, void *data, bool delete_all, int(*list_compare)(void *a, void *b));
void list_clear(list_t *list);
void list_foreach(list_t *list, void(*list_func)(void *a));
void list_delete(list_t *list);

#endif /* _LIST_H_ */
