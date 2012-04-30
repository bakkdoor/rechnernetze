
#ifndef PERSON_LIST_H
#define PERSON_LIST_H

struct node {
  struct node *next;
  
  char *firstname;
  char *lastname;
};

int insert_node(const char *firstname, const char *lastname);
int remove_node(const char *firstname, const char *lastname);
void remove_all_nodes();

void print_list();

#endif
