///////////////////////////////////////////////////////////////////////////////
//test sigprocmask
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main()
{
	sigset_t     set;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGTERM);
	sigprocmask(SIG_SETMASK, &set, NULL);//屏蔽SIGINT信号
	//sigprocmask(SIG_SETMASK, NULL, NULL);//清空所有屏蔽信号

	while(1)
	{
		printf("I am running\n");
		sleep(1);
	}

	return 0;
}

/*
sigprocmask测试
把sigprocmask参数的how设置为SIG_SETMASK,set信号集里增加SIG_INT，来屏蔽SIG_INT信号。
1）首先运行程序：./sigprocmask
2) 然后另开一个终端用ps命令查询sigprocmask程序的进程ID：ps -ef|grep sigprocmask
3）向进程发送SIGINT信号，kill -SIGINT 7771(这里假设sigprocmask进程ID为7771)
4) 会发现sigprocmask程序还在运行：继续输出“I am running”
5) 去掉注释sigprocmask(SIG_SETMASK, &set, NULL)这句代码，重复1-4步进程结束，进程结束
hehe,如果在向set增加SIGTERM，那就更神奇了。
*/


























