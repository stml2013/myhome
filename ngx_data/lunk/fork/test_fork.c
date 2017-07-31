#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/*fork一般的用法*/
void normal_fork()
{
     pid_t pid;

    pid = fork();
    if (pid > 0)
    {
        printf("parent process is over\n");
        //do something
    }
    else if (pid < 0)
    {
        printf("fork is error\n");
        exit(-1);
    }
    else
    {
        printf("child is processing \n");
        //do something
    }

    for (;;) {}

}

/*daemon*/
void daemon_fork()
{
    pid_t pid;

    pid = fork();
    if (pid > 0)
    {
        printf("parent is exit\n");
        exit(0);//留下子进程来处理问题
    }
    else if (pid < 0)
    {
        printf("fork is failed\n");
        exit(-1);
    }
    else
    {
		if (setsid() == -1)
		{
			printf("setsid is failed\n");
		}

		umask(000);
	
        printf("child is working\n");

		FILE *fp = fopen("test.txt", "a");
		if (fp == NULL)
		{
			kill(pid, SIGTERM);
		}
        //do something
        int i = 0;
        
        for (;;)
        {
			fprintf(fp, "%s", (u_char*)("I am the deamon two\n"));
            fprintf(fp, "i = %d\n", ++i);

            sleep(10);
			
			if (i > 10)
			{
				fclose(fp);
				kill(pid, SIGTERM);
			}			
        }
    }
}

int main()
{
/*    pid_t pid;

    pid = fork();
    if (pid > 0)
    {
        printf("parent process is over\n");
       // exit(0);
    }
    else if (pid < 0)
    {
        printf("fork is error\n");
        exit(-1);
    }
    else
    {
        printf("child is processing \n");

//        int i = 0;
//        for (;;)
//        {
//            printf("i = %d\n", ++i);
//            sleep(1);
//        }
    }

    for (;;) {}
*/
	
	daemon_fork();
	
	return 0;
}
