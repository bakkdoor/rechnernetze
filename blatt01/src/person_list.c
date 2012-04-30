#include <stdlib.h>
#include <stdio.h>
#include "mystrings.h"
#include "person_list.h"

// this is the global person linked list (initialized to NULL => empty)
struct node *mylist = NULL;

// represent the #nodes in the list (for debugging or somethink else)
// will be set automaticly while insert or remove nodes (do not set this manually!)
int node_count = 0;

// insert new node into mylist (linked list). 
// Returns the position of the new created node or 0 if fail.
int insert_node(const char *firstname, const char *lastname)
{
  int pos = 1;
  struct node *newnode;
  struct node *next = mylist;
  struct node *prev = NULL;
  
  // allocate memory for the new node
  newnode = (struct node *) malloc(sizeof(newnode));
  if (newnode == NULL) return 0; // not enough memory, skip here
  
  // allocate memory for the data (fist-, lastname)
  newnode->firstname = (char *)calloc(strlen(firstname) + 1, sizeof(char));
  newnode->lastname = (char *)calloc(strlen(lastname) + 1, sizeof(char));
  if (newnode->firstname == NULL || newnode->lastname == NULL) return 0;
  
  // copy data
  strcpy(newnode->firstname, firstname);
  strcpy(newnode->lastname, lastname);
  
  if (mylist == NULL) 
  {
    // the list is empty, set new node as list head
    newnode->next = NULL;
    mylist = newnode;
    return ++node_count;
  }
  
  // search insert position (alphabeticly insertion)
  while (next != NULL)
  {
    if (strcmp(next->lastname, lastname) > 0 || 
      (strcmp(next->lastname, lastname) == 0 && strcmp(next->firstname, firstname) >= 0))
    {
      // found insert position between two nodes in the list
      newnode->next = next;
      
      if (prev == NULL)
      {
	mylist = newnode;
      } else {
	prev->next = newnode;
      }
            
      node_count++;
      return pos;
    }
    
    // iteration step
    pos++;
    prev = next;
    next = next->next;
  }
  
  // insert as last element in the list
  prev->next = newnode;
  node_count++;
  return ++pos;
}

// Remove node from the list, where the first- and lastname
// are equals to the given.
// Returns the old node position or 0 if fail.
int remove_node(const char *firstname, const char *lastname)
{
  int pos = 1;
  struct node *next = mylist;
  struct node *prev = NULL;
  
  while (next != NULL)
  {    
    if (strcmp(next->lastname, lastname) == 0 && strcmp(next->firstname, firstname) == 0)
    {
      // 'next' is my node to remove 
      // remove...
      if (prev == NULL)
      {
	mylist = next->next;
      } else {
	prev->next = next->next;
      }
      free(next->firstname);
      free(next->lastname);
      free(next);
      
      node_count--;
      return pos;
    }
    
    // iteration step
    pos++;
    prev = next;
    next = next->next;
  }
  
  // there is no node with the given last- and firstname!
  return 0;
}

// Clear mylist.
void remove_all_nodes()
{  
  // remove list head until the list is empty
  while(mylist != NULL)
  {
    remove_node(mylist->firstname, mylist->lastname);
  }
}

// Print all nodes to std out.
void print_list()
{
  struct node *next = mylist;
  
  // iterate over the list and print node data to stdout
  while (next != NULL) 
  {
    printf("firstname: %s, lastname: %s\n", next->firstname, next->lastname);
    next = next->next;
  }
}

// Main function.
// Read the names from given file (promt), 
// create nodes for each entry (first- and lastname) and put them in a sorted linked list.
// Print the whole list to std out.
int main(int argc, const char **argv)
{
  char firstname[100], lastname[100], filename[255];
  FILE *namefile = NULL;
  
  // open the name file
  while (namefile == NULL)
  {
    printf("Path to the name-file: \n");
    scanf("%255s", filename);
    
    if (strcmp(filename, "exit") == 0) {
      printf("buy!\n");
      return EXIT_SUCCESS;
    }
    
    printf("Read %s...\n", filename);
    
    namefile = fopen(filename, "r");
    if (namefile == NULL)
    {
      printf("The file %s does not exist or you hasn't read permission! Try again or type 'exit'.\n", filename);
    }
  }
  
  // file opened, read data
  while (!feof(namefile))
  {
    if (fscanf(namefile, "%100s %100s\n", firstname, lastname) == 2)
    {
      // printf("firstname: %s, lastname: %s\n", firstname, lastname);
      insert_node(firstname, lastname);
    }
  }
  
  // close file
  fclose(namefile);
  
  printf("\nSorted list (on lastname than firstname):\n");
  print_list();
  remove_all_nodes();
  printf("buy!\n");
  
  return EXIT_SUCCESS;
}

