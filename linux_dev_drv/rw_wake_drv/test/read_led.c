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
int fd;

void* read_proc(void* context)
{
    printf("enter read_proc...\n");
    sleep(1);
    read(fd, buf, 4);
    // if main thread cancel this read thread successfully,
    // never hit here.
    printf("exit read_proc\n");
    sleep(1);
    return NULL;
}

int main(void)
{
    pthread_t thr;
    int rc;

	fd = open("/dev/rw_wake", O_RDWR);
	if (fd < 0) {
		perror("open /dev/rw_wake");
	}

    rc = pthread_create(&thr, NULL, read_proc, NULL);
    printf("iinitiate read thread %d ...\n", rc);
    sleep(4);
    printf("tyr to cancel read_thread...\n");
    rc = pthread_cancel(thr);
    printf("rc = %d\n", rc);
    rc = pthread_join(thr, NULL);
    printf("read complete! sleep and then exit\n");
    sleep(4);
    close(fd);
    return 0;
}
