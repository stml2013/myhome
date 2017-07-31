#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <utime.h>

int main()
{
	int fd;
	int read_len;
	char buf[1024];

	struct utimbuf ut;

	if ((fd = open("/home/wyp/桌面/copy", O_RDWR)) == -1)
	{
		printf("error\n");
	}
	else
	{
		printf("seccess\n");
		read_len = read(fd, buf, 1024);
		buf[read_len] = '\0';
		printf("%s\n", buf);
	}

	if ((read_len = readlink("/home/wyp/桌面/copy", buf, 1024)) == -1)
	{
		printf("error\n");
		exit(-1);
	}

	buf[read_len] = '\0';
	printf("%s\n", buf);

	ut.actime = 0;
	ut.modtime = 0;

    if (utime("/home/wyp/桌面/copy", &ut) == -1)
    {
        printf("error");
        exit(-1);
    }

	return 0;
}
