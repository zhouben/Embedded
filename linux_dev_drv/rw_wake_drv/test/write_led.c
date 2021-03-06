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

#define BUF_SIZE 256
char buf[BUF_SIZE] = {0};

int main(void)
{
	int fd;

	fd = open("/dev/imx28x_led", O_RDWR);
	if (fd < 0) {
		perror("open /dev/imx283_led");
	}

	printf("test write....");
    write(fd, buf, BUF_SIZE); 
    printf("\t\tcomplete!\n");
    close(fd);
    return 0;
}
