#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "shared_state.h"

int main(void)
{
	int fd = 0;
	int ret = 0;
  unsigned int i = 0;
  bool result = true;
  void* ptr = NULL;

  if ((ret = posix_memalign(&ptr, 4096, 4096)) < 0)
    return ret;

	fd = open("/dev/ukm", O_RDONLY, 0);

	if ((ret = ioctl(fd, UKM_MAP_MEMORY, ptr)) < 0)
  {
    printf("Error - driver failed to map memory\n");
    goto exit;
  }

  memset(ptr, 'A', 4096);

  if ((ret = ioctl(fd, UKM_CHECK_BUFFER, NULL)) < 0)
  {
    printf("Error - driver does not see A's\n");
    goto unmap;
  }

  if ((ret = ioctl(fd, UKM_MUTATE, NULL)) < 0)
  {
    printf("Error - driver failed mutate ioctl\n");
    goto unmap;
  }

  for (i = 0; i < 4096; i++)
    result &= (((char*)ptr)[i] == 'A');
  if (!result)
    printf("Error - driver failed to mutate or flush changes\n");

unmap:
  if ((ret = ioctl(fd, UKM_UNMAP_MEMORY, NULL)) < 0)
    printf("Error - driver failed to unmap memory\n");
exit:
  free(ptr);
	return ret;
}
