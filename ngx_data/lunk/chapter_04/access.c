#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
	const char *pFile = "test.txt";

	if (argc == 2) pFile = argv[1];

	if (access(pFile, F_OK) == -1)
    {
		printf("File don't exit\n");
		exit(-1);
    }

	if (access(pFile, R_OK) == 0)
		printf("Can read the file\n");

	if (access(pFile, W_OK) == 0)
		printf("Can write the file\n");
    
	if (access(pFile, X_OK) == 0)
		printf("Can execute the file\n");

	return 0;
}
