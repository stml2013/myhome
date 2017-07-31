#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/stat.h>

typedef struct 
{
	pid_t     child_pid;
	char      message[PIPE_BUF+1];
}fifo_message;

int main()
{
	int                     fd;
	const char             *fifoname;
	fifo_message            msgbuf;

	fifoname = "/tmp/serverfifo";

	if (access(fifoname, F_OK) == -1)
	{
		if (mkfifo(fifoname, 0666) == -1)
		{
			perror("mkfifo error\n");
			exit(-1);
		}
	}

	if ((fd = open(fifoname, O_RDONLY)) == -1)//以只读、阻塞模式打开
	{
		fprintf(stdout, "open %s failed\n", fifoname);
		exit(-1);
	}

	while (1)
	{
		if (read(fd, &msgbuf, sizeof(msgbuf)) < 0)
		{
			close(fd);
			perror("read error\n");
			exit(-1);
		}

		fprintf(stdout, "message from child: %d, message: %s\n", msgbuf.child_pid, msgbuf.message);
		sleep(1);
	}
    
	return 0;
}
