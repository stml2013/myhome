#include <string.h>
#include <stdio.h>
#include <ctype.h>

int main()
{
	int  c;
	//用户输入数据
	while ((c = getchar()) != EOF)
	{
		if (isupper(c))
		{
			c = tolower(c);
		}
		//输出数据作为testinput的输入数据
		if (putchar(c) == EOF)
		{
			fputs("output error\n", stdout);
		}
		
		if (c == '\n')
		{
			fflush(stdout);
		}
	}
	
	return 0;
}
