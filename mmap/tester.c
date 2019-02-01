#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/mman.h>
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
  char* memory = NULL; 
  void* mmap_res = NULL;

  printf("Tester allocating memory\n");
  memory = aligned_alloc(4096, 4096*4);
  if (!memory)
  {
    printf("Error - failed to allocate memory\n");
    goto exit;
  }

  fd = open("/dev/ukm", O_RDWR, 0);

  printf("Driver allocating memory\n");
  if ((ret = ioctl(fd, UKM_ALLOC_MEMORY, 0)) < 0)
  {
    printf("Error - driver failed to allocate and map memory\n");
    goto closefd;
  }

  printf("Replacing 2nd page with kernel memory\n");
  mmap_res = mmap(&memory[4096*2], // Second half of memory
    4096*2,                 // Two pages
    PROT_WRITE | PROT_READ, // R/W
    MAP_SHARED | MAP_FIXED, // Force overwrite of mappings
    fd,                     // kernel module file descriptor
    0);                     // base (page index 0 and 1 will be used)

  if (mmap_res == MAP_FAILED)
  {
    ret = -1;
    perror("mmap error");
    goto driver_cleanup;
  }

  // Mutate to all A's
  memset(&memory[4096*2], 'A', 4096*2);

  if ((ret = ioctl(fd, UKM_VERIFY)) < 0)
  {
    printf("Error - driver failed to verify memory\n");
    goto cleanup;
  }

  if ((ret = ioctl(fd, UKM_MUTATE)) < 0)
  {
    printf("Error - driver failed to mutate memory\n");
    goto cleanup;
  }

  for (i = 0; i < (4096*2); i++)
  {
    if (memory[((4096*2) + i)] != 'B')
    {
      printf("Error - driver didn't mutate memory correctly\n");
      ret = -1;
      break;
    }
  }

cleanup:
  // Unmap the kernel memory, re-map with anonymous memory
  munmap(&memory[4096*2], 4096*2);
  mmap_res = mmap(&memory[4096*2], 4096*2, PROT_WRITE | PROT_READ,
    MAP_ANONYMOUS, -1, 0);

driver_cleanup:
  if ((ret = ioctl(fd, UKM_FREE_MEMORY, 0)) < 0)
    printf("Error - driver failed to unmap and free memory\n");
  else
    printf("Driver Freed memory\n");

closefd:
  close(fd);
freemem:
  free(memory);
  if (ret == 0)
    printf("Test Successful\n");
  else
    printf("Test failed\n");
exit:
  return ret;
}
