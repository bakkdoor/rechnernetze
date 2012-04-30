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

void person_delete(void *data)
{
  person_t *person = (person_t*)data;
  if(person) {
    free(person->firstname);
    free(person->lastname);
    free(person);
  }
}

void person_print(void *x)
{
  person_t *person = (person_t*)x;
  printf("%s %s\n", person->firstname, person->lastname);
}

void print_help(char *programm_name)
{
  printf("Usage:\n%s <names_file>\n", programm_name);
}

bool person_insert(void *a, void *b)
{
  person_t *p_a = (person_t*)a;
  person_t *p_b = (person_t*)b;
  return (strcmp(p_b->lastname, p_a->lastname) > 0 ||
          (strcmp(p_b->lastname, p_a->lastname) == 0 && strcmp(p_b->firstname, p_a->firstname) >= 0));
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
        list_insert_after(list, person, &person_insert);
      }
    }
  }

  fclose(file);
  return list;
}

int main(int argc, char **argv)
{
  list_t *names;
  char filename[255];

  /* if(argc < 2) { */
  /*   print_help(argv[0]); */
  /*   exit(1); */
  /* } */

  printf("Enter filename: ");
  scanf("%255s", filename);
  names = read_names_file(filename);

  printf("\nSORTED:\n");
  printf("===========\n");
  list_foreach(names, &person_print);

  list_delete(names, &person_delete);
  return 0;
}
