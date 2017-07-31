#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

void err_exit(const char *message)
{
	printf("%s", message);
	exit(-1);
}

int main()
{
/*
	struct stat Stat;

	if (stat("test.txt", &Stat) == -1)
   	{
		printf("stat error\n");
		exit(-1);
	}

	printf("");
*/

	const char *pBuf = "I can do it!\n";
	int fd;

	if ((fd = open("test.txt", O_RDWR)) == -1)
	{
		err_exit("open error\n");
	}

	if (lseek(fd, 1024, SEEK_SET) == -1)
	{
		err_exit("lseek error\n");
	}

	printf("%d\n", strlen(pBuf));
	if (write(fd, pBuf, strlen(pBuf)) == -1)
	{
		err_exit("write error\n");
	}

	close(fd);

	return 0;
}


