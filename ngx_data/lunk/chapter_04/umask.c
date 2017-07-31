#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main()
{
	int  fd;
	int pid;

	if ((pid = fork()) > 0)
	{
		umask(0033);
	    fd = open("utChild.txt", O_RDWR|O_CREAT, 0777);
		if (fd == -1)
	    {
			printf("open error\n");
			exit(-1);
        }
	}
	else if (pid == 0)
	{
		/*子进程的umask值不会影响父进程的umask*/
		//umask(0000);
	    fd = open("utParent.txt", O_RDWR|O_CREAT, 0777);
		if (fd == -1)
	    {
			printf("open error\n");
			exit(-1);
        }
	}
	else 
	{
		printf("fork error\n");
		exit(-1);
	}
	
	return 0;
}
