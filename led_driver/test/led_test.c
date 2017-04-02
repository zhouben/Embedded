#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <limits.h>
#include <asm/ioctls.h>
#include <time.h>
#include <pthread.h>

char buf[1] = {0};

int main(void)
{
	int fd;

	fd = open("/dev/imx28x_led", O_RDWR | O_NONBLOCK);
	if (fd < 0) {
		perror("open /dev/imx283_led");
	}

	printf("test write.... buf %08X\n", (unsigned int)buf);
	buf[0] = 0;

	write(fd, buf, 1);
	sleep(2);

	buf[0] = 1;
	write(fd, buf, 1);
	sleep(1);	

	printf("test ioctl..... \n");
	ioctl(fd, 0);
	sleep(2);
	ioctl(fd, 1);
}
