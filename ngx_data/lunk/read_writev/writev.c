#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>

int main()
{
	struct iovec   iov[3];
	ssize_t        bytes_write;
	int            fd;
	int            i;

	char *buf[] = {
		"ssize_t readv(int fd, const struct iovec *iov, int iovcnt);",
		"ssize_t writev(int fd, const struct iovec *iov, int iovcnt);",
		"The readv() system call reads iovcnt buffers from the  file  associated with the file \
         descriptor fd into the buffers described by iov (\"scatter input\")"
	};

    if ((fd = open("test.txt", O_WRONLY | O_CREAT, 0644)) == -1)
	{
		perror("open error\n");
		return -1;
	}

	for (i = 0; i < 3; ++i)
	{
		iov[i].iov_base = buf[i];
		iov[i].iov_len  = strlen(buf[i]);
	}

	bytes_write = writev(fd, iov, 3);
	if (bytes_write == -1)
	{
		perror("writev error\n");
		return -1;
	}

	printf("wrote %d bytes\n", bytes_write);

	if (close(fd))
	{
		perror("close\n");
		return -1;
	}

	return 0;
}




































