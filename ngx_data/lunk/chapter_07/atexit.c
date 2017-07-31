#include <stdio.h>
#include <stdlib.h>

static void fun1();
static void fun2();

int main()
{
    if (atexit(fun1) != 0)
    {
        printf("fun1 error\n");
        exit(-1);
    }

    if (atexit(fun2) != 0)
    {
        printf("fun2 error");
        exit(-1);
    }

    return 0;
}

static void fun1()
{
    printf("fun1 is called\n");
}

static void fun2()
{
    printf("fun2 is called\n");
}










































