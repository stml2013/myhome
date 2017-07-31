#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void sig_handler(int sig)
{
    switch(sig)
    {
    case 23:
         printf("child : the signo is 23, hehe\n");
         return;
    case 22:
         printf("father : hello wordl 22!\n");
         return;
    }
}

int main()
{
    struct sigaction act, oact;
    int    status;

    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler   = sig_handler;
    sigaction(23, &act, &oact);
    sigaction(22, &act, &oact);

    pid_t pid, ppid;

    if (!(pid = fork()))
    {
        printf("child begin\n");

        kill(getppid(), 22);
        sleep(3);

        printf("child over\n");
    }
    else
    {
        printf("father begin\n");

        kill(getpid(), 23);

        wait(&status);

        if (WIFSIGNALED(status))
        {
            printf("child process receive siganl %d\n", WTERMSIG(status));
        }

        printf("father over\n");
    }

    return 0;
}
