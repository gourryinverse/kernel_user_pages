#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <libexplain/ioctl.h>

#include "shared_state.h"

int main(void)
{
	int fd = 0;
	int ret = 0;
  unsigned int i = 0;
  bool result = true;
  void* ptr = NULL;
  char * curchar = malloc(sizeof(char));
  *curchar = 'A';

  if ((ret = posix_memalign(&ptr, 4096, 4096)) < 0)
    return ret;

	fd = open("/dev/ukm", O_RDONLY, 0);

	if ((ret = ioctl(fd, UKM_MAP_MEMORY, ptr)) < 0)
  {
    printf("Error - driver failed to map memory\n");
    goto exit;
  }
  printf("Driver mapped memory\n");

  while (true)
  {
    memset(ptr, *curchar, 4096);
    if ((ret = ioctl(fd, UKM_CHECK_BUFFER, curchar)) < 0)
    {
      printf("Error - driver does not see userland mutation\n");
      goto unmap;
    }

    (*curchar)++;
    if ((ret = ioctl(fd, UKM_MUTATE, curchar)) < 0)
    {
      printf("Error - driver failed mutate ioctl.\n");
      goto unmap;
    }

    for (i = 0; i < 4096; i++)
      result &= (((char*)ptr)[i] == (*curchar));
    if (!result)
    {
      printf("Error - userland does not see kernel land mutation\n");
      goto unmap;
    }
    (*curchar)++;
  }
unmap:
  if ((ret = ioctl(fd, UKM_UNMAP_MEMORY, NULL)) < 0)
    printf("Error - driver failed to unmap memory\n");
exit:
  free(ptr);
  free(curchar);
	return ret;
}
