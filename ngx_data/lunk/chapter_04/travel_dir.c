/*要学一门脚本语言编程*/

#include <stdio.h>
#include <dirent.h>
#include <limits.h>

tyoedef int MyFunc(const char *, const struct stat *, int);

static MyFunc myfunc;
static int    myftw(char *, MyFunc*);
static int    dopath(MyFunc *);
static void   err_exit(const char *);

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

int main(int argc, char *argv[])
{
    int ret;

    if (argc != 2) err_exit("usage: ftw<start-pathname> \n");

    ret = myftw(argv[1], myfunc);

    ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
    if (ntot == 0) ntot = 1;
}

static int err_exit(const char *message)
{
    printf("%s", message);
    exit(-1);
}
