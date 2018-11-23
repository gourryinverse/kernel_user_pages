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

MODULE_LICENSE("GPL");
#define UKM_MINOR MISC_DYNAMIC_MINOR

typedef spinlock_t SpinLock;
SpinLock global_mutex;

char* shared_data = NULL;
struct page* user_page = NULL;

static int driver_map_memory(void* arg)
{
  int ret = 0;
  if ((ret = get_user_pages_fast((unsigned long)arg,
    1, 1, &user_page)) < 0)
    return ret;
  if ((shared_data = kmap(user_page)) == NULL)
    return -1;
  return 0;
}

static int driver_check_buffer(void)
{
  bool result = true;
  unsigned int i;
  if (!shared_data)
    return -1;

  for (i = 0; i < 4096; i++)
    result &= (shared_data[i] == 'A');

  return result ? 0 : -1;
}

static int driver_mutate(void)
{
  // mutate each byte in shared_data to "C"
  if (shared_data)
    memset(shared_data, 'B', 4096);
  SetPageDirty(user_page);
  return 0;
}

static int driver_unmap_memory(void)
{
  // put_user_pages on shared_buffer
  if (shared_data)
  {
    kunmap(user_page);
    shared_data = NULL;
  }
  if (user_page)
  {
    put_page(user_page);
    user_page = NULL;
  }
  return 0;
}

static long MainDeviceIoctl(struct file* file, unsigned int ioctl, unsigned long arg)
{
  int ret = 0;
  enum driver_args cmd = (enum driver_args) ioctl;
  switch (cmd)
  {
    case UKM_MAP_MEMORY:
      ret = driver_map_memory((void*)arg);
      break;
    case UKM_CHECK_BUFFER:
      ret = driver_check_buffer();
      break;
    case UKM_MUTATE:
      ret = driver_mutate();
      break;
    case UKM_UNMAP_MEMORY:
      ret = driver_unmap_memory();
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
