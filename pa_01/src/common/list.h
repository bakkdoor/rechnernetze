#ifndef _LIST_H_
#define _LIST_H_

#include <sys/types.h>
#include <stdbool.h>

/*
  Private struct definitions
 */

typedef struct node {
  struct node *next;
  void *data;
} list_node_t;

typedef struct list {
  list_node_t *first;
  size_t size;
} list_t;

/*
 Public List API
*/

/**
 * @return New empty list.
 *
 * Creates a new list.
 **/
list_t * list_new(void);

/**
 * @param delete_func Function to be called with each element in list.
 *                    Ignored, if NULL. Should be used for cleanup of elements, if necessary.
 *
 * Deletes a list (frees its memory).
 **/
void list_delete(list_t * list, void(* delete_func)(void * data));

/**
 * @return Size of list / Amount of elements in list.
 * Return the size (amount of elements) in list.
 **/
size_t list_size(const list_t * list);

/**
 * @param list List to add an element to.
 * @param data Data to be added as an element to list.
 **/
void list_insert(list_t * list, void * data);

/**
 * @param list List to add data to.
 * @param data Data to be added to list.
 * @param predicate Boolean function that returns true if data should be inserted after another set of data in list.
                    Used to determine position after which other element to insert data to.
 * @return Index of insertion.
 **/
unsigned int list_insert_after(list_t * list, void * data, bool (* predicate)(void * a, void * b));

/**
 * @param list List to remove data from.
 * @param data Data/Element to be removed from list.
 * @param list_compare Compare function. Removes all elements from list for which it returns 0.
 * @param delete_func Delete function for elements (can be NULL).
 **/
void list_remove(list_t * list, void * data, bool delete_all, int (* list_compare)(void * a, void * b), void (* delete_func)(void * data));

/**
 * @param list List to clear.
 * @param delete_func Delete function for elements (can be NULL).
 **/
void list_clear(list_t * list, void (* delete_func)(void * data));

/**
 * @param list List to loop over.
 * @param list_func Iteration function called with each element data in list.
 **/
void list_foreach(const list_t * list, void (* list_func)(void * a));

/**
 * @param list List to be sorted.
 * @param list_compare Compare function used for sorting.
 **/
void list_sort(list_t * list, int (* list_compare)(const void * a, const void * b));

/**
 * @param list List to be filtered from.
 * @param predictate Predicate function used to filter elements from list.
 * @return New list with all elements in list for which predicate returns true.
 **/
list_t * list_filter(const list_t * list, bool (* predicate)(const void * a));

/**
 * @param list List to be mapped over.
 * @param func Function yielding new value for mapped list.
 * @return List of all return values of calling func on each element in list.
 **/
list_t * list_map(const list_t * list, void * (* func)(const void * a));

/**
 * @param list List to find element in.
 * @param compare_func Function returning true for elements that match search.
 * @return First element in list for which compare_func returns true.
 **/
void * list_find_first(const list_t * list, bool (* compare_func)(const void * a));

bool list_contains(const list_t * list, void * elem);

#endif /* _LIST_H_ */
