#include <unistd.h>
#include <signal.h>
#include <stdio.h>

void sig_handler(int sig)
{
	if (sig == SIGINT) 
		printf("SIGINT sig\n");
    else if (sig == SIGQUIT)
		printf("SIGQUIT sig\n");
    else if (sig == SIGUSR1)
		printf("SIGUSR1 sig\n");
    else if (sig == SIGUSR2)
		printf("SIGUSR2 sig\n");
    else 
		printf("SIGCHLD sig\n");
}

int main()
{
	int                i;
    sigset_t           new;
	sigset_t           old;
	sigset_t           wait;
    struct sigaction   act;

    act.sa_handler = sig_handler;
	act.sa_flags   = 0;

    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, 0);
    sigaction(SIGQUIT, &act, 0);
    sigaction(SIGUSR1, &act, 0);
	sigaction(SIGUSR2, &act, 0);
	sigaction(SIGCHLD, &act, 0);

    sigemptyset(&new);
    sigaddset(&new, SIGINT);

    sigemptyset(&wait);
    sigaddset(&wait, SIGUSR1);
	sigaddset(&wait, SIGUSR2);

    sigprocmask(SIG_BLOCK, &new, &old);

	for (i=0; i < 90; ++i)
	{
		printf("%d\n", i);
		sleep(1);
	}

    sigsuspend(&wait);

    printf("After sigsuspend\n");

	printf("yyyy\n");
    if (-1 == sigprocmask(SIG_SETMASK, &old, NULL)) 
    {
        perror("sigprocmask");
        exit(-1);
    } 
	for (i=0; i < 30; ++i)
	{
		printf("%d\n", i);
		sleep(1);
	}
	printf("xxxxx\n");

    return 0;

}
