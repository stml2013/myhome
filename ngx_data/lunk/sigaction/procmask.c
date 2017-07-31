////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void sig_handler(int signum)
{
	switch(signum)
    {
    case SIGINT:
         printf("the signo is SIGINT %d received, hehe\n", SIGINT);
         return;
    case SIGCHLD:
         printf("the signo is SIGCHLD %d received, hihi!\n", SIGCHLD);
         return;
	case SIGUSR1:
         printf("the signo is SIGUSR1 %d received, hihi!\n", SIGUSR1);
         return;
	case SIGIO:
         printf("the signo is SIGIO %d received, hihi!\n", SIGIO);
         return;
	case SIGALRM:
		 printf("the signo is SIGALRM %d received, hihi!\n", SIGALRM);
         return;
	default:
		 printf("the signo %d is not processed\n", signum);
		 return;
    }
}

int main()
{
	int              i;
	int              status;
	pid_t            pid;
	sigset_t         set;
	sigset_t         oldset;
	struct sigaction act;
	struct sigaction oldact;

	act.sa_flags   = 0;
	act.sa_handler = sig_handler;
	sigemptyset(&act.sa_mask);
	sigaction(SIGINT, &act, &oldact);
	sigaction(SIGCHLD, &act, &oldact);
	sigaction(SIGUSR1, &act, &oldact);
	sigaction(SIGALRM, &act, &oldact);
	sigaction(SIGIO, &act, &oldact);

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGUSR1);

	if ((pid = fork()) < 0)
	{
		fprintf(stdout, "fork error\n");
		exit(-1);
	}
	else if (pid > 0)
	{
		if (sigprocmask(SIG_BLOCK, &set, &oldset) < 0)//屏蔽信SINGINT、SIGUSR1
		{
			fprintf(stdout, "sigprocmask error\n");
			kill(pid, SIGKILL);
			exit(-1);
		}
/*
		waitpid(pid, &status, 0);

		if (WIFSIGNALED(status))
        {
            printf("child process receive siganl %d\n", WTERMSIG(status));
        }
*/
		pause();//接收SIGIO?
		pause();//接收SIGALRM?
		pause();//接收SIGCHLD?
		//pause();
		//pause();
	}
	else
	{
		sleep(1);
		kill(getppid(), SIGINT);//信号被屏蔽
		sleep(1);
		kill(getppid(), SIGIO);
		sleep(1);
		kill(getppid(), SIGUSR1);//信号被屏蔽
		sleep(1);
		kill(getppid(), SIGALRM);
		sleep(2);
		//子进程退出会发送一个SIGCHLD信号
	}
	
	return 0;
}
