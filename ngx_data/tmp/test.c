//#include <iostream>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
   // bool flag;
    int num;
    char ch;
}mystruct;

typedef struct
{
    int  *p1;
    int  *p2;
    char *p3;
}ngxs;

int main()
{
    int *a[3][4];

    freopen("save.data", "r", stdin);
    printf("%d\n", sizeof(mystruct));
    printf("%d\n", sizeof(ngxs));

    return 0;
}
