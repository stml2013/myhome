#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void set_fl(int fd, int flags)
{
	int val;
	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
	{
		printf("fcntl F_GETFL error\n");
        exit(-1);
	}

	val |= flags;

	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
	{
		printf("fcntl F_SETFL error\n");
        exit(-1);
	}
}


void clr_fl(int fd, int flags)
{
	int val;
	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
	{
		printf("fcntl F_GETFL error\n");
        exit(-1);
	}

	val &= ~flags;

	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
	{
		printf("fcntl F_SETFL error\n");
        exit(-1);
	}
}

void test()
{
	set_fl(STDOUT_FILENO, O_SYNC);
}













































