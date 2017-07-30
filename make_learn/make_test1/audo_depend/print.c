#include "print.h"

void print(const char *str)
{
   if (str == NULL) 
   {
      printf("Empty String\n");
   }
   else
   {
      printf("%s\n", str);
   }
}
