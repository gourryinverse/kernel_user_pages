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
#include <linux/vmalloc.h>
#pragma GCC diagnostic pop

#include "./shared_state.h"

MODULE_LICENSE("PROPRIETARY");
#define UKM_MINOR MISC_DYNAMIC_MINOR

#define VM_RESERVED (VM_DONTEXPAND | VM_DONTDUMP)

typedef spinlock_t SpinLock;
SpinLock global_mutex;

#define NUM_PAGES 2
#define MEMSIZE (4096 * NUM_PAGES)
char* vmalloc_ptr;

static int driver_alloc_memory(void)
{
  vmalloc_ptr = NULL;
  if (!(vmalloc_ptr = vmalloc(MEMSIZE)))
    return -1;
  return 0;
}

static int driver_verify(void)
{
  int result = 0;
  unsigned int i;
  // Verify that the contents of vmapped_ptr are 'A'
  for (i = 0; i < MEMSIZE; i++)
  {
    if (vmalloc_ptr[i] != 'A')
    {
        result = -1;
        break;
    }
  }
  return result;
}

static int driver_mutate(void)
{
  memset(vmalloc_ptr, 'B', MEMSIZE);
  return 0;
}

static int driver_free_memory(void)
{
  vfree(vmalloc_ptr);
  vmalloc_ptr = NULL;
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
    case UKM_VERIFY:
      ret = driver_verify();
      break;
    case UKM_MUTATE:
      ret = driver_mutate();
      break;
    case UKM_FREE_MEMORY:
      ret = driver_free_memory();
      break;
    default:
      break;
  }
  return ret;
}

static void MainDeviceMemoryMapOpen(struct vm_area_struct* vma)
{
  vma->vm_private_data = NULL;
}

static void MainDeviceMemoryMapClose(struct vm_area_struct* vma)
{
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,11,0)
static int MainDeviceMemoryMapFault(struct vm_area_struct* vma, struct vm_fault* vmf)
{
  vmf->page = vmalloc_to_page(vmalloc_ptr + (4096 * (vmf->pgoff)));
  if (vmf->page)
    get_page(vmf->page);
  else
    return VM_FAULT_SIGBUS;
  return 0;
}
#else
static int MainDeviceMemoryMapFault(struct vm_fault* vmf)
{
  vmf->page = vmalloc_to_page(vmalloc_ptr + (4096 * (vmf->pgoff)));
  if (vmf->page)
    get_page(vmf->page);
  else
    return VM_FAULT_SIGBUS;
  return 0;
}
#endif

struct vm_operations_struct mainDeviceVmOps =
{
  .open = MainDeviceMemoryMapOpen,
  .close = MainDeviceMemoryMapClose,
  .fault = MainDeviceMemoryMapFault
};

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
  vma->vm_ops = &mainDeviceVmOps;
  vma->vm_flags |= VM_RESERVED;
  vma->vm_private_data = file->private_data;
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
