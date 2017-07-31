//在Linux系统中有132个错误编码。我们可以写一个简单的小程序来测试系统中的错误编码对应的说明：

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define ERRO_NUM 132

int main()
{
	int i;

	for(i = 0; i < ERRO_NUM; i++)
	{
		printf("%d:%s\n", i, strerror(i));
	}

    return 0;
}
