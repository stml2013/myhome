#include <unistd.h>

int pipe(int filedse[2]);
filedes返回两个文件描述符；
filedse[0]为读打开，filedse[1]为写打开， filedse[1]的输出是filedes[0]的输入。

管道的一个例子：
//////////////////////////////////////////////////////////////////////////
父进程向子进程传送数据。
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <apue.h>

#define  MAXLINE  100
int main()
{
	int      n;
	int      fd[2];
	pid_t    pid;
	char     line[MAXLINE];
	
	if (pipe(fd) < 0)
	{
		err_sys("pipe error\n");
	}
	
	if ((pid = fork()) < 0)
	{
		err_sys("fork error\n");
	}
	else if (pid > 0)
	{
		close(fd[0]);
		write(fd[1], "hello world\n", sizeof("hello world\n"));
	}
	else
	{
		close(fd[1]);
		n = read(fd[0], lile, MAXLINE);
		write(STDOUT_FILENO, line, n);
	}
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <apue.h>
#include <sys/wait.h>

#define DEF_PAGER  "/bin/more"
#define MAXLINE    100

int main(int argc, char *argv[])
{
	int      n;
	int      fd[2];
	pid_t    pid;
	char    *pager
	char    *argv0;
	char     line[MAXLINE];
	FILE    *fp;
	
	if (argc < 2)
	{
		err_sys("usage: a.out <pathname>");
	}
	
    if ((fp = fopen(argv[1], "r")) == NULL)
	{
			err_sys("can't open %s", argv[1]);
			return 1;
	}
		
	if (pipe(fd) < 0)
	{
		err_sys("pipe error\n");
	}
	
	if ((pid = fork()) < 0)
	{
		err_sys("fork error\n");
	}
	else if (pid > 0)
	{
		close(fd[0]);
		
		while (fgets(line, MAXLINE, fp) != NULL)
		{
			n = strlen(line);
			if (write(fd[1], line, n) != n)
			{
			    err_sys("write error\n");
			}
		}
		
		if (ferror(fp))
		{
			err_sys("fgets error\n");
		}
		
		if (waitpid(pid, NULL, 0) < 0)
		{
			err_sys("waitpid error\n");
		}
		
		fclose(fp);
		close(fd[1]);
		
		exit(0);
	}
	else
	{
		close(fd[1]);
		
		if (fd[0] != STDIN_FILENO)
		{
			if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)
			{
				err_sys("dup2 error\n");
			}
			
			close(fd[0]);
		}
		
		if ((pager = getenv("PAGER")) == NULL)
		{
			pager = DEF_PAGER;
		}
		
		if ((argv0 = strrchr(pager, '/')) != NULL)
		{
			++argv0;
		}
		else
		{
			argv0 = pager;
		}
		
		if (execl(pager, argv0, (char*)0) < 0)
		{
			err_sys("execl error\n");
		}
	}
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
FILE *popen(const char *cmdstring, const char *type);
int pclose(FILE *fp);
popen创建一个管道连接到另一进程，然后读其输出或向输入写数据。
例如：
FILE* fpin  = popen("test.c", "r");
FILE* fpout = popen("test.c", "w");

#include <stdio.h>
#include <sys/wait.h>

#define PAGER "${PAGER:-more}"
#define MAXLINE 100

int main()
{
	char     line[MAXLINE];
	FILE    *fpin;
	FILE    *fpout;
	
	if (argc != 2)
	{
		err_quit("usage: a.out <pathname>");
	}
	if ((fpin = fopen(argv[1], "r"));
	{
		err_sys("can't open %s", argv[1]);
		exit(0);
	}
	
	if ((fpout = popen(PAGER, "w") == -1)
	{
		err_sys("popen error\n");
		fclose(fpin);
	}
	
	while (fgets(line, MAXLINE, fpin) != NULL)
	{
		if (fputs(line, fpout) == EOF)
		{
			err_sys("fputs error\n");
		}
	}
	
	if (ferror(fpin)) 
	{
		err_sys("fgets error\n");
	}
	
	if (pclose(fpout) == -1)
	{
		err_sys("pclose error\n");
	}
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
#include <apue.h>
#include <ctype.h>

int main()
{
	int  c;
	
	while ((c = getchar()) != EOF)
	{
		if (isupper(c))
		{
			c = tolower(c);
		}
		
		if (putchar(c) == EOF)
		{
			sys_says("output error\n");
		}
		
		if (c == '\n')
		{
			fflush(stdout);
		}
	}
	
	return 0;
}

#include <apue.h>
#include <sys/wait.h>

#define MAXLINE 100

int main()
{
	char   line[MAXLINE];
	FILE  *fpin;
	
	if ((fpin = popen("mytest", "r")) == NULL)
	{
		err_sys("popen error\n");
	}
	
	for ( ; ; )
	{
		fputs("prompt> ", stdout);
		fflush(stdout);
		
		if (fgets(line, MAXLINE, fpin) == NULL)
		{
			break;
		}
		
		if (fputs(line, stdout) == EOF)
		{
			err_sys("fputs error to pipe");
		}
	}
	
	if (pclose(fpin) == -1)
	{
		err_sys("pclose error\n");
	}
	
	puts("");
	
	exit(0);
}

////////////////////////////////////////////////////////////////////////////////////////////3
#include <stdio.h>
#include <string.h>
#include <apue.h>
#incluce <unistd.h>

#define MAXSIZE 200

int main()
{
	FILE    *fpin;
	FILE    *fpout;
	char     line[MAXSIZE+1];
	
	memset(buffer, '\0', sizeof(line));
	
	fpin  = popen("ls -l", "r");
	if (fpin == NULL)
	{
		err_sys("popen fpin error\n");

		return -1;
	}

	fpout = popen("grep *.c", "w");
	if (fpout == NULL)
	{
		err_sys("popen fpout error\n");
		pclose(fpin);

        return -1;
	}
	//fgets失败或者读到文件未返回NULL
	while (fgets(line, MAXSIZE, fpin) != NULL)
	{	//fputs成功返回非负，失败返回EOF
		if (fputs(line， fpout) == EOF)
		{
			err_sys("fputs error\n");
		}
	}
	
	pclose(fpin);
    pclose(fpout);

	return 0;
}


