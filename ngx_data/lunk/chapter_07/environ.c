#include <stdio.h>
#include <stdlib.h>

void err_exit(const char *vMsg)
{
    printf("vMsg");
    exit(-1);
}

int main()
{
    const char* pValue = NULL;

    if (putenv("MYNAME=WYP") != 0)
    {
        err_exit("putenv error\n");
    }

    if ((pValue = getenv("MYNAME")) == NULL)
    {
        err_exit("getenv error\n");
    }

    printf("%s\n", pValue);

    return 0;
}
