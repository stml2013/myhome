/*
系统调用dup和dup2能够复制文件描述符。dup返回新的文件文件描述符（没有用的文件描述符最小的编号）。
dup2可以让用户指定返回的文件描述符的值，如果需要，则首先接近newfd的值，通常用来重新打开或者重
定向一个文件描述符。
原型如下：
#include <unsitd.h>
int dup(int oldfd);
int dup2(int oldfd,int newfd);
dup 和dup2都是返回新的描述符。或者返回-1并设置 errno变量。新老描述符共享文件的偏移量
（位置）、标志和锁，但是不共享close-on-exec标志。
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	int fd0;
	int s_fd;
	int n_fd;
	fd0 = open("test.txt", 0666);
	
	if (fd0 < 0)
	{
		printf("open error\n");
		exit(-1);
	}

	s_fd = dup(STDOUT_FILENO);
	if (s_fd < 0)
	{
		printf("err in dup\n");
	}

	printf("hello world1\n");

	n_fd = dup2(fd0, STDOUT_FILENO);//重定向标准输出到文件fd0
	if (n_fd < 0)
	{
		printf("err in dup2\n");
	}
	
	write(STDOUT_FILENO, "hello world2", sizeof("hello world2"));
	puts("i am out");

	if (dup2(s_fd, n_fd) < 0)//恢复重点向到标准输出
	{
		printf("xxxxx\n");
	}
	
	write(STDOUT_FILENO, "hello world3\n", sizeof("hello world3\n"));

	close(fd0);

	return 0;
}





