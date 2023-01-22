#include <stdio.h>
#include <stdlib.h>

int main () {
   printf("Witaj %s!\n", getenv("LOGNAME"));
   return 0;
}
