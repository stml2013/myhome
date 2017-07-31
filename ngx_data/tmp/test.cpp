#include <iostream>
#include <stdio.h>
#include <stdlib.h>

struct
{
    int num;
    char ch;
    bool flag;
}type_one;

struct
{
    bool flag;
    int num;
    char ch;
}type_two;

int main()
{
    int *a[3][4];
    printf("%d\n", sizeof(type_one));
    printf("%d\n", sizeof(type_two));
    printf("%d\n", sizeof(a));

    return 0;
}
