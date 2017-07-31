#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int main()
{
	pid_t   pid;
	FILE   *fp;

	if (access("var/run/lock.pid", R_OK) == 0)
	{
		printf("Existing a copy of this data\n");
		exit(1);
  	}

	pid = fork();
	
	if (pid > 0)
	{
		printf("parent is over!\n");
		fp = fopen("var/run/lock.pid", "wt");
		
		if (fp == NULL)
		{
			printf("can open the file\n");
			sleep(9);
			kill(pid, SIGTERM);//kill子进程
			exit(-1);
		}
		
		fprintf(fp, "%d", pid);
		fclose(fp);
		exit(0);
	}
	else if (pid < 0)
	{
		printf("fork is failed!\n");
		exit(-1);
	}
    else
	{
		for ( ;; )
		{
			printf("daemon is running\n");
			sleep(3);
		}
	}
	
	return 0;
}









