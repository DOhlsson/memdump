#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
  printf("My pid is %d\n", getpid());
  char* str = "This is my string, my very special string";
  long* bignum = malloc(sizeof(long));
  *bignum = 0xEFBEADDE;

  int* i = malloc(sizeof(int));
  while ((*i) < 100) {
    if ((*i) % 10 == 0) {
      printf("%s\n", str);
    } else if ((*i) % 10 == 1) {
      printf("My very nice and big number: %ld\n", *bignum);
    }
    printf("%d\n", *i);
    sleep(1);
    (*i)++;
  }

  free(bignum);
  free(i);
  return 0;
}
