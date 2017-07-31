#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAXSIZE 200

void err_sys(const char *str);

int main()
{
	FILE    *fpin;
	FILE    *fpout;
	char     line[MAXSIZE+1];
	
	memset(line, '\0', sizeof(line));
	
	fpin  = popen("ls -l", "r");
	if (fpin == NULL)
	{
		err_sys("popen fpin error\n");

		return -1;
	}

	fpout = popen("grep .c", "w");
	if (fpout == NULL)
	{
		err_sys("popen fpout error\n");
		pclose(fpin);

        return -1;
	}
	//fgets失败或者读到文件未返回NULL
	while (fgets(line, MAXSIZE, fpin) != NULL)//从第一次中取出数据
	{	//fputs成功返回非负，失败返回EOF
		if (fputs(line, fpout) == EOF)//将取出的数据作为第二次命令的输入
		{
			err_sys("fputs error\n");
		}
	}
	
	pclose(fpin);
    pclose(fpout);

	return 0;
}

void err_sys(const char *str)
{
	printf("%s", str);
}
