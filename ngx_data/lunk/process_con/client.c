#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

struct fifo_message
{
	pid_t      child_pid;
	char       message[PIPE_BUF+1];
};


int main()
{
	int                     fd;
	const char             *fifoname;
	struct fifo_message     msgbuf;

	fifoname = "/tmp/serverfifo";
	if (access(fifoname, F_OK) == -1)
	{
		perror("access error\n");
		exit(-1);
	}

	if ((fd = open(fifoname, O_WRONLY)) < 0)//以只写、阻塞模式打开
	{
		perror("open error\n");
	}

	msgbuf.child_pid = getpid();

	while (1)
	{
		printf("input the message: ");
		if (fgets(msgbuf.message, sizeof(msgbuf.message), stdin) == NULL)
		{
			perror("fgets error or end\n");
			break;
		}

		msgbuf.message[strlen(msgbuf.message)] = '\0';

		if (write(fd, &msgbuf, sizeof(msgbuf)) == -1)
		{
			perror("write error\n");
			close(fd);
			exit(-1);
		}
	}
	
	close(fd);

	return 0;
}
