#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main(void)
{
	int fd;
	int ret;
	int val;
	int null;
	fd = open("/dev/interrupt_test", O_RDONLY, 0);

	while(1)
	{
		ret = ioctl(fd, 0x0, &null);
	}
	return ret;
}
