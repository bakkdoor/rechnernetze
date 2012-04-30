#include <stdlib.h> // for atoi()
#include <stdio.h>  // for printing to stdout
#include <ctype.h>  // for toupper()
#include "mystrings.h"

// Compare two strings: 
// On equal strings, return 0.
// If the first differ character of 'str1' is alphabeticly lower 
// than the one from 'str2', returns a value < 0.
// Otherwise, the returned value is > 0.
int strcmp(const char *str1, const char *str2)
{
  while ('\0' != *str1 && '\0' != *str2)
  {      
    if (toupper(*str1) - toupper(*str2))
    {
      return *str1 - *str2;
    }
    str1++;
    str2++;
  }
  
  return toupper(*str1) - toupper(*str2);
}

// Copy the source string (included '\0') into 
// destination string given by a pointer.
// Returns destination pointer.
char *strcpy(char *destination, const char *source)
{
  int i;
  if (destination == NULL) return NULL;
  
  for (i = 0; i < strlen(source); i++)
  {
    destination[i] = source[i];
  }
  
  destination[i] = '\0';
  return destination;
}

// Returns str-string length (without '\0')
size_t strlen(const char *str)
{
  size_t count = 0;
  if (str == NULL) return 0;
  while(str[count] != '\0') count++; 
  
  return count;
}

/*
// Test string functions (strcmp(): compare two strings,
//                        strlen(): return string length, without '\0'
//			  strcpy(): copies the source string to destination)
int main(int args, char** argv) 
{
  char *tmp;
  
  if (args != 3)
  {
    printf("Please give me two strings to compare!\n");
    printf("Usage: %s compare-string1 compare-string2\n", argv[0]);
    return EXIT_SUCCESS;
  }
  
  printf("strcmp(%s, %s) = %d\n", argv[1], argv[2], strcmp(argv[1], argv[2]));
  
  tmp = (char *)calloc(strlen(argv[1]) + 1, sizeof(char));
  
  
  
  printf("length of '%s' is %d\n", argv[1], strlen(argv[1]));
  printf("this three strings should be the same: \n\tsource:\t%s\n\tdest1:\t%s\n\tdest2:\t%s\n", argv[1], strcpy(tmp, argv[1]), tmp);
  
  free(tmp);
  printf("\n");
  return EXIT_SUCCESS;
}
*/


