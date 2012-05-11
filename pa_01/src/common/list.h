#ifndef _LIST_H_
#define _LIST_H_

#include <sys/types.h>
#include <stdbool.h>

/*
 Public List API
*/

struct list;
typedef struct list list_t;

list_t * list_new(void);
void list_delete(list_t * list, void(* delete_func)(void * data));
size_t list_size(const list_t * list);
void list_insert(list_t * list, void * data);
unsigned int list_insert_after(list_t * list, void * data, bool (* predicate)(void * a, void * b));
void list_remove(list_t * list, void * data, bool delete_all, int (* list_compare)(void * a, void * b), void (* delete_func)(void * data));
void list_clear(list_t * list, void (* delete_func)(void * data));
void list_foreach(const list_t * list, void (* list_func)(void * a));
void list_sort(list_t * list, int (* list_compare)(void * a, void * b));
list_t * list_filter(const list_t * list, bool (* predicate)(void * a));
list_t * list_map(const list_t * list, void * (* func)(void * a));
void * list_find_first(const list_t * list, bool * (* compare_func)(void * a));
#endif /* _LIST_H_ */
