#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define  MAXLINE  100

void err_sys(const char *str);

int main()
{
	int      n;
	int      status;
	int      fd[2];
	pid_t    pid;
	char     line[MAXLINE];
	
	if (pipe(fd) < 0)
	{
		err_sys("pipe error\n");
	}
	
	if ((pid = fork()) < 0)
	{
		err_sys("fork error\n");
	}
	else if (pid == 0)
	{
		close(fd[0]);
		if (write(fd[1], "hello world\n", sizeof("hello world\n")) == -1)
		{
			err_sys("write error\n");
			
			exit(-1);
		}
	}
	else 
	{
		if (waitpid(pid, &status, 0) != pid)
		{
			err_sys("waitpid error\n");
		}

		if (WIFEXITED(status) == 0)
		{
			err_sys("child process exit failed\n");
		}

		close(fd[1]);
		n = read(fd[0], line, MAXLINE);
		write(STDOUT_FILENO, line, n);
	}
	
	
	return 0;
}

void err_sys(const char *str)
{
	printf("%s", str);
}


