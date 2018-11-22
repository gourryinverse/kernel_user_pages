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

MODULE_LICENSE("GPL");
#define UKM_MINOR MISC_DYNAMIC_MINOR

typedef spinlock_t SpinLock;
SpinLock global_mutex;

static long MainDeviceIoctl(struct file* file, unsigned int ioctl, unsigned long arg)
{
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
