#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
int main(void)
{
  unsigned int size = 0x20000000;
  unsigned int page = 0;
  char* mem = NULL;
  mem = malloc(size);
  if (!mem)
    return -1;
  while (true)
  {
    for (page = 0; page < (size/4096); page++)
       memset(&mem[page*4096], 'A', 4096);
  }
  return 0;
}
