#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/semaphore.h>
#include <linux/ioctl.h>
#include <linux/version.h>
#include <linux/highmem.h>
#include <linux/mm.h>
#pragma GCC diagnostic pop

#include "./shared_state.h"

MODULE_LICENSE("PROPRIETARY");
#define UKM_MINOR MISC_DYNAMIC_MINOR

typedef spinlock_t SpinLock;
SpinLock global_mutex;

#define NUM_PAGES 4
struct page* page_structs[NUM_PAGES];
char* vmalloc_ptr;

static int driver_alloc_memory(void)
{
  unsigned int i;
  int success = -1;
  vmalloc_ptr = NULL;
  memset(page_structs, '\0', sizeof(page_structs));

  if (!(vmalloc_ptr = vmalloc(4096 * NUM_PAGES)))
    goto exit;

  for (i = 0; i < NUM_PAGES; i++)
   page_structs[i] =  virt_to_page(vmalloc_ptr + (i*4096));

  success = 0;
exit:
  return success;
}

static int driver_test(void)
{
  unsigned int i, j;
  // Write to all of that memory space
  for (i = 0; i < NUM_PAGES; i++)
  {
    for (j = 0; j < 4096; j++)
      vmalloc_ptr[((i * 4096) + j)] = 'A';
  }
  return 0;
}

static int driver_free_memory(void)
{
  vfree(vmalloc_ptr);
  vmalloc_ptr = NULL;
  memset(page_structs, '\0', sizeof(page_structs));
  return 0;
}

static long MainDeviceIoctl(struct file* file, unsigned int ioctl,
  unsigned long arg)
{
  int ret = -1;
  enum driver_args cmd = (enum driver_args) ioctl;
  switch (cmd)
  {
    case UKM_ALLOC_MEMORY:
      ret = driver_alloc_memory();
      break;
    case UKM_TEST:
      ret = driver_test();
      break;
    case UKM_FREE_MEMORY:
      ret = driver_free_memory();
      break;
    default:
      break;
  }
  return ret;
}

static int MainDeviceOpen(struct inode* inode, struct file* file)
{
  return 0;
}

static int MainDeviceRelease(struct inode* inode, struct file* file)
{
  return 0;
}

static int MainDeviceMemoryMap(struct file* file, struct vm_area_struct* vma)
{
  return 0;
}


struct file_operations mainDeviceOps =
{
  .owner = THIS_MODULE,
  .unlocked_ioctl = MainDeviceIoctl,
  .compat_ioctl = MainDeviceIoctl,
  .open = MainDeviceOpen,
  .release = MainDeviceRelease,
  .mmap = MainDeviceMemoryMap
};

struct miscdevice mainDevice =
{
  UKM_MINOR,
  "ukm",
  &mainDeviceOps
};


static int UKMInit(void)
{
  spin_lock_init(&global_mutex);
  return misc_register(&mainDevice);
}


static void UKMExit(void)
{
  misc_deregister(&mainDevice);
}

module_init(UKMInit);
module_exit(UKMExit);
