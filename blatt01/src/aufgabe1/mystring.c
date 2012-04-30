#include <stdlib.h> // for atoi()
#include <stdio.h>  // for printing to stdout
#include <ctype.h>  // for toupper()

#include "mystring.h"

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
