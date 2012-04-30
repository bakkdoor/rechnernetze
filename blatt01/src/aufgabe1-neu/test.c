#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "mystring.h"

typedef struct person {
  char *firstname;
  char *lastname;
} person_t;

person_t *person_new(char *firstname, char *lastname) {
  person_t *person = malloc(sizeof(person_t));
  if(!person) {
    return NULL;
  }
  person->firstname = (char *)calloc(strlen(firstname) + 1, sizeof(char));
  person->lastname = (char *)calloc(strlen(lastname) + 1, sizeof(char));
  // copy data
  strcpy(person->firstname, firstname);
  strcpy(person->lastname, lastname);
  return person;
}

void print_person(void *x)
{
  person_t *person = (person_t*)x;
  printf("%s %s\n", person->firstname, person->lastname);
}

void print_help(char *programm_name)
{
  printf("Usage:\n%s <names_file>\n", programm_name);
}

list_t *read_names_file(char *filename)
{
  list_t *list;
  FILE *file;
  person_t *person;
  char firstname[100], lastname[100];

  file = fopen(filename, "r");
  if(!file) {
    printf("Could not open file: %s\nQuitting.\n", filename);
    exit(1);
  }

  list = list_new();
  while(!feof(file)) {
    if(fscanf(file, "%100s %100s\n", firstname, lastname) == 2) {
      if((person = person_new(firstname, lastname))) {
        list_insert(list, person);
      }
    }
  }

  fclose(file);
  return list;
}

void sort_names(list_t *names)
{
}

int main(int argc, char **argv)
{
  list_t *names;

  if(argc < 2) {
    print_help(argv[0]);
    exit(1);
  }

  names = read_names_file(argv[1]);

  printf("NOT SORTED:\n");
  printf("===========\n");
  list_foreach(names, &print_person);

  sort_names(names);

  printf("\nSORTED:\n");
  printf("===========\n");
  list_foreach(names, &print_person);

  list_delete(names);
  return 0;
}
