/*
ssize_t readv(int fd, const struct iovec *iov, int count);
从文件描述符fd中读取count个段到count个buffer iov中
*/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>

int main()
{
	struct iovec   iov[3];
	ssize_t        bytes_read;
	int            fd;
	int            i;
	char           buf[3][512];

    if ((fd = open("test.txt", O_RDONLY)) == -1)
	{
		perror("open error\n");
		return -1;
	}

	for (i = 0; i < 3; ++i)
	{
		memset(buf[i], 0, sizeof(buf[i]));
		iov[i].iov_base = buf[i];
		iov[i].iov_len  = sizeof(buf[i]);
	}

	bytes_read = readv(fd, iov, 3);
	if (bytes_read == -1)
	{
		perror("writev error\n");
		return -1;
	}

	for (i = 0; i < 3; ++i)
	{
		printf("%d %s\n", i, (char*)iov[i].iov_base);
	}

	printf("read %d bytes\n", bytes_read);

	if (close(fd))
	{
		perror("close\n");
		return -1;
	}

	return 0;
}





































