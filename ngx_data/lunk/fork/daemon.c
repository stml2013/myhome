#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>


int main()
{
	int   count;
	int   flag;
    pid_t pid;

    pid = fork();

    if (pid > 0)
    {
        printf("parent process is run\n");
		flag = 1;
    }
    else if (pid < 0)
    {
        printf("fork is error\n");
        exit(-1);
    }
    else
    {
        printf("child is run \n");
		flag = 0;
    }

    count = 0;
	
	if (flag)
	{
		printf("count in parent is : %d\n", ++count);
	}
	else
	{
		printf("count in child is : %d\n", ++count);
	}
	
	
	return 0;
}
