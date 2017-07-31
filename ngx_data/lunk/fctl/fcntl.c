/*
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int main()
{
	int   fd;
	int   var;

	if ((fd = open("test.txt", O_RDWR | O_CREAT | O_EXCL, 0664)) == -1)
	{
		perror("open error");
		exit(1);
	}

	if ((var = fcntl(fd, F_GETFL, 0)) < 0)
	{
		perror("fcntl error");
		close(fd);
		exit(1);
	}

	switch(var & O_ACCMODE)
	{
		case O_RDONLY:
			printf("Read only..\n");
			break;
		case O_WRONLY:
			printf("Write only..\n");
			break;
		case O_RDWR:
			printf("Read And Write..\n");
			break;
		default:
			printf("Do't know..\n");
			break;
	}

	if (var & O_APPEND)
	{
		printf("And Append..\n");
	}

	if (var & O_NONBLOCK)
	{
		printf("And Nonblocking..\n");
	}

	if (close(fd) == -1)
	{
		perror("close error");
	}
	
	return 0;
}
*/


/*
//读写锁测试
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	int              fd;
	int              fret;
	struct flock     rdflk;
	struct flock     wrflk;
	
	///*
	关于加锁：
	1）如果文件是以只读打开的，只能加读锁，加写锁时会失败返回-1
	2）如果文件是以只写打开的，只能加写锁，加读锁时会失败返回-1
	3）如果文件是以读写打开的，在同一个进程中，如果同时加了两种锁，后加的锁会覆盖前面加的锁。
	   比如先加写锁，后加读锁，结果文件加上的是读锁；如果先加读锁，后加写锁，文件最终加上的是写锁。
	//*

	if ((fd = open("test.txt", O_RDWR)) == -1)
	{
		perror("open error");
		exit(1);
	}

	if ((fret = fcntl(fd, F_GETFD)) == -1)
	{
		perror("fcntl error");
		exit(1);
	}

	fcntl(fd, F_SETFD, 1);//在exec时关闭

	rdflk.l_type   = F_RDLCK;
	rdflk.l_whence = SEEK_SET;
	rdflk.l_start  = 0;
	rdflk.l_len    = 100;
	wrflk          = rdflk;
	wrflk.l_type   = F_WRLCK;
	//文件先加读锁
	if (fcntl(fd, F_SETLK, &rdflk) == -1)
	{
		perror("fcntl set rdflk error");
		exit(1);
	}
	//后加写锁，最终写锁会覆盖读锁，文件加的是写锁
	if (fcntl(fd, F_SETLK, &wrflk) == -1)
	{
		perror("fcntl set wrflk error");
		exit(1);
	}

	return 0;
}
*/

/*
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	int              fd;
	int              fret;
	struct flock     rdflk;
	struct flock     wrflk;

	//关于加锁：
	//1）如果文件是以只读打开的，只能加读锁，加写锁时会失败返回-1
	if ((fd = open("test.txt", O_RDONLY)) == -1)
	{
		perror("open error");
		exit(1);
	}

	rdflk.l_type   = F_RDLCK;
	rdflk.l_whence = SEEK_SET;
	rdflk.l_start  = 0;
	rdflk.l_len    = 100;
	wrflk          = rdflk;
	wrflk.l_type   = F_WRLCK;
	//加写锁，会出现错误，因为文件是以只读打开的
	//fcntl set wrflk error: Bad file descriptor
	if (fcntl(fd, F_SETLK, &wrflk) == -1)
	{
		perror("fcntl set wrflk error");
		exit(1);
	}

	return 0;
}
*/

/*
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	int              fd;
	int              fret;
	struct flock     rdflk;
	struct flock     wrflk;

	rdflk.l_type   = F_RDLCK;
	rdflk.l_whence = SEEK_SET;
	rdflk.l_start  = 0;
	rdflk.l_len    = 100;
	wrflk          = rdflk;
	wrflk.l_type   = F_WRLCK;

	//关于加锁：
	//2）如果文件是以只写打开的，只能加写锁，加读锁时会失败返回-1
	if ((fd = open("test.txt", O_WRONLY)) == -1)
	{
		perror("open error");
		exit(1);
	}
	
	//加读锁，会出现错误，因为文件是以只写打开的
	//fcntl set wrflk error: Bad file descriptor
	if (fcntl(fd, F_SETLK, &rdflk) == -1)
	{
		perror("fcntl set wrflk error");
		exit(1);
	}

	return 0;
}
*/

//以读写方式打开，在不同进程之间加锁
/*
当一个共享锁被set到一个文件的某段的时候，其他的进程可以set共享锁到这个段或这个段的一部分。共享锁阻止任何其他进程set独占锁到这段保护区的任何部分。如果文件描述符没有以读的访问方式打开，共享锁的设置请求会失败。
独占锁阻止任何其他的进程在这段保护区域任何位置设置共享锁或独占锁。如果文件描述符不是以写的方式打开，独占锁的请求会失败。
总结：如果有一个进程加写锁，其他进程不能加任何锁；如果是读锁，其他进程只能加读锁。
*/
/*
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define BUF_SIZE 512

int main()
{
	int              fd;
	int              bytes_read;
	int              status;
	pid_t            pid;
	const char      *chbuf;
	char             buf[BUF_SIZE];
	struct flock     tmflk;
	struct flock     rdflk;
	struct flock     wrflk;

	chbuf          = "xxxxx";
	rdflk.l_type   = F_RDLCK;
	rdflk.l_whence = SEEK_SET;
	rdflk.l_start  = 0;
	rdflk.l_len    = 100;
	wrflk          = rdflk;
	tmflk          = rdflk;
	wrflk.l_type   = F_WRLCK;

	//在进程之间以读写方式打开，在不同进程之间加读写锁
	if ((fd = open("test.txt", O_RDWR | O_APPEND)) == -1)
	{
		perror("open error");
		exit(1);
	}
	
	if ((pid = fork()) < 0)
	{
		perror("fork error");
		exit(1);
	}
	else if (pid > 0)
	{	//父进程加读锁
		if (fcntl(fd, F_SETLK, &wrflk) == -1)
	    {
			perror("parent fcntl set rdflk error");
			exit(1);
	   	}
		//然子进程尝试加锁
		sleep(1);
		//写入数据
		if (write(fd, chbuf, sizeof(chbuf))  == -1)
		{
			perror("parent write error");
		}

		tmflk        = rdflk;
		tmflk.l_type = F_UNLCK;
		//然子进程尝试申请锁，以便测试在有读锁的情况下，其他进程不能加锁
		sleep(3);
		//释放锁
		if (fcntl(fd, F_SETLK, &tmflk) == -1)
		{
			perror("fcntl unlck error");
		}

		waitpid(pid, &status, 0);//等待子进程返回
	}
	else
	{
		sleep(1);
		
		tmflk = rdflk;
		//尝试枷锁
		while (1)
		{   //获取文件锁的信息
			if (fcntl(fd, F_GETLK, &tmflk) == -1)
			{
				perror("child fcntl getlk error");
			}

			if (tmflk.l_type == F_UNLCK)
			{
				break;
			}

			if (tmflk.l_type == F_RDLCK)
			{
				printf("process Id:%d has locked with F_RDLCK\n", tmflk.l_pid);
			}
			else if (tmflk.l_type == F_WRLCK)
			{
				printf("process Id:%d has locked with F_WRLCK\n", tmflk.l_pid);
			}

			sleep(1);
		}
		//加读锁
		if (fcntl(fd, F_SETLK, &rdflk) == -1)
		{
			perror("fcntl setlk rdflk error");
			exit(1);
		}
	
		printf("child fcntl get the rdflk\n");
		
		lseek(fd, rdflk.l_whence, rdflk.l_start);//重定位
		if((bytes_read = read(fd, buf, BUF_SIZE)) < 0)
		{
			perror("read error");
			exit(1);
		}

		buf[bytes_read-1] = '\0';
		write(STDOUT_FILENO, buf, bytes_read);
		printf("\n");

		tmflk        = rdflk;
		tmflk.l_type = F_UNLCK;
		
		fcntl(fd, F_SETLK, &tmflk);
	}

	close(fd);

	return 0;
}
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define BUF_SIZE 256

int main(int argc, char *argv[])
{
	int               fd;
	int               status;
	pid_t             pid;

	char              buf[BUF_SIZE];
	struct flock      tmflk;
	struct flock      rdflk;
	struct flock      wrflk;

	if ((fd = open("test.txt", O_RDWR | O_APPEND)) == -1)
	{
		perror("open error");
		exit(1);
	}
	
	rdflk.l_type    = F_RDLCK;
	rdflk.l_whence  = SEEK_SET;
	rdflk.l_start   = 0;
	rdflk.l_len     = 100;
	tmflk           = rdlck;
	tmflk.l_type    = F_UNLCK;
	wrflk           = rdlck;
	wrflk.l_type    = F_WRLCK;
	
	
	if (fcntl(fd, F_SETLK, &rdlck) == -1)
    {
		perror("fcntl set rdlck error");
		exit(1);
	}

	if (fcntl(fd, F_GETLK, &tmlck) == -1)
	{
		perror("fcntl get lock error");
		exit(1);
	}

	if (tmlck.l_type != F_UNLCK)
	{
		if (tmlck.l_type == F_RDLCK)
		{
			printf("process id %d has locked the file with F_RDLCK\n", tmlck.l_pid);
		}
		else if (tmlck.l_type == F_WRLCK)
		{
			printf("process id %d has locked the file with F_WRLCK\n", tmlck.l_pid);
		}
	}

	tmlck        = rdlck;
	tmlck.l_type = F_UNLCK;

    if (fcntl(fd, F_SETLK, &tmplck) == -1)
	{
		perror("fcntl set F_UNLCK error");
	}

	if (fcntl(fd, F_SETLK, &wrlck)
	{
		perror("fcntl set wrlck error");
	}

	close(fd);

	return 0;
}



































