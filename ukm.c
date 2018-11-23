#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/semaphore.h>
#include <linux/ioctl.h>
#include <linux/version.h>
#pragma GCC diagnostic pop

#include "./shared_state.h"

MODULE_LICENSE("GPL");
#define UKM_MINOR MISC_DYNAMIC_MINOR

typedef spinlock_t SpinLock;
SpinLock global_mutex;

struct shared_state* shared_data;

bool checker_active;

static void driver_map_memory(void* arg)
{
  struct map_memory_args* args = (struct map_memory_args*)arg;
  void* user_ptr = NULL;
  (void) user_ptr;
  (void) args;
  // map user memory
  // copy user ptr
  // unmap user memory
  // get_user_page(user_ptr)
  // store the now permanently mapped pointer globally?
  // set user_mem->mapped = true

  return;
}

static void driver_check_buffer(bool* arg)
{
  bool result = false;
  unsigned int i;
  (void) arg;
  if (!shared_data)
    goto exit;

  for (i = 0; i < 4096; i++)
    result &= (shared_data->data[i] == 'C');

exit:
  // write_to_user(arg, sizeof(result, result))
  // copy result to arg->result;
  return;
}

static void driver_mutate(void)
{
  // mutate each byte in shared_data to "C"
  if (shared_data)
    memset(shared_data, 'C', 4096);
  return;
}

static void driver_unmap_memory(void)
{
  // put_user_pages on shared_buffer
  return;
}

static long MainDeviceIoctl(struct file* file, unsigned int ioctl, unsigned long arg)
{
  enum driver_args cmd = (enum driver_args) ioctl;
  switch (cmd)
  {
    case UKM_MAP_MEMORY:
      driver_map_memory((void*)arg);
      break;
    case UKM_CHECK_BUFFER:
      driver_check_buffer((bool*)arg);
      break;
    case UKM_MUTATE:
      driver_mutate();
      break;
    case UKM_UNMAP_MEMORY:
      driver_unmap_memory();
      break;
    default:
      break;
  }
	return 0;
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
