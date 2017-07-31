#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <utime.h>

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("too few argv\n");
        exit(-1);
    }

    int    i,fd;
    struct stat statbuf;
    struct utimbuf utb;

    for (i = 1; i < argc; ++i)
    {
        if (stat(argv[i], &statbuf) < 0)
        {
            printf("%s: stat error\n", argv[i]);
            continue;
        }

        if ((fd = open(argv[1], O_RDWR|O_TRUNC)) == -1)
        {
            printf("%s: open error\n", argv[i]);
            continue;
        }
        close(fd);

        utb.actime  = statbuf.st_atime;
        utb.modtime = statbuf.st_mtime;
        if (utime(argv[i], &utb) < 0)
        {
            printf("%s: utime error\n", argv[i]);
            continue;
        }
    }

    return 0;
}
