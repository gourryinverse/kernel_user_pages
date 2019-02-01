#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <libexplain/ioctl.h>

#include "shared_state.h"

int main(void)
{
  int fd = 0;
  int ret = 0;
  unsigned int i = 0;
  bool result = true;

  fd = open("/dev/ukm", O_RDONLY, 0);

  if ((ret = ioctl(fd, UKM_ALLOC_MEMORY, 0)) < 0)
  {
    printf("Error - driver failed to allocate and map memory\n");
    goto exit;
  }
  printf("Driver mapped memory - beginning test\n");

  for (i = 0; i < 0x10; i++)
  {
    if ((ret = ioctl(fd, UKM_TEST, 0)) < 0)
      break;
    sleep(1);
  }
  printf("finished testing\n");

  if ((ret = ioctl(fd, UKM_FREE_MEMORY, 0)) < 0)
    printf("Error - driver fialed to unmap and free memory\n");

  printf("Freed memory\n");

exit:
  close(fd);
  return ret;
}
