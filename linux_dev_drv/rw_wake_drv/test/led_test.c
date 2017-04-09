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

	fd = open("/dev/rw_wake", O_RDWR | O_NONBLOCK);
	if (fd < 0) {
		perror("open /dev/rw_wake");
	}

    printf("try to read ...\n");
    sleep(1);
    read(fd, buf, 1);
    printf("read complete! sleep and then exit\n");
    sleep(1);
    close(fd);
    return 0;
}
