/*
kill（传送信号给指定的进程）
相关函数 raise，signal
表头文件 #include<sys/types.h>
#include<signal.h>
定义函数 int kill(pid_t pid,int sig);
函数说明
kill()可以用来送参数sig指定的信号给参数pid指定的进程。参数
pid有几种情况:
pid>0 将信号传给进程识别码为pid 的进程。
pid=0 将信号传给和目前进程相同进程组的所有进程
pid=-1 将信号广播传送给系统内所有的进程
pid<0 将信号传给进程组识别码为pid绝对值的所有进程
参数sig代表的信号编号
返回值 执行成功则返回0，如果有错误则返回-1。
错误代码：
EINVAL 参数sig 不合法
ESRCH 参数pid 所指定的进程或进程组不存在
EPERM 权限不够无法传送信号给指定进程
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void hello()
{
    printf("xxxxxx\n");
}

int main()
{
	pid_t   pid;
	int     status;
	pid_t   ppid;
	if (!(pid = fork()))//这是子进程
	{
        printf("Hi i am child process!\n");
        sleep(10);
        hello();
        sleep(30);//保证子进程能够收到父进程的发送的sig信息
        hello();
        return;
	}
	else//父进程
	{
        ppid = getppid();
        printf("ppid %d  pid %d\n", ppid, pid);
        printf("send signal to child process %d\n", pid);
        sleep(1);
        kill(pid, SIGABRT);
        wait(&status);

        if (WIFSIGNALED(status))
        {
            printf("child process receive siganl %d\n", WTERMSIG(status));
        }
	}

	return 0;
}


