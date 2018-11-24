#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
int main(void)
{
  unsigned int i = 0;
  unsigned int size = 0x80000000;
  unsigned int page = 0;
  char* mem = NULL;
  while (true)
  {
    mem = malloc(size);
    if (!mem)
      return -1;
    for (page = 0; page < (size/4096); page++)
      mem[i * 4096] = 'A';
    free(mem);
    mem = NULL;
  }
  return 0;
}
