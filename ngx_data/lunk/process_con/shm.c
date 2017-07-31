#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <error.h>

#define MEMORY_SIZE 1024

int main()
{
	int               wpid;
	int               status;
	int               failed;
	int               shmid;
	char             *addr;
	pid_t             pid;
	struct shmid_ds   buf;

	failed = 0;

	shmid  = shmget(IPC_PRIVATE, MEMORY_SIZE, IPC_CREAT|0600);
	if (shmid == -1)
	{
		perror("shmget error");
		return -1;
	}

	pid = fork();
	if (pid == 0)
	{
		addr = (char*)shmat(shmid, NULL, 0);
		if ((int)addr == -1)
		{
			perror("shmat addr error");
			return -1;
		}

		strcpy(addr, "I am the child process\n");

		shmdt(addr);
		
		return 3;
	}
	else if (pid > 0)
	{
		wpid = waitpid(pid, &status, 0);
		if (wpid > 0 && WIFEXITED(status))
		{
			printf("child process return is %d\n", WEXITSTATUS(status));
		}

		failed = shmctl(shmid, IPC_STAT, &buf);
		if (failed == -1)
		{
			perror("chmctl error");
			return -1;
		}
		
		printf("shm_segsz =%d bytes\n", buf.shm_segsz);
        printf("parent pid=%d, shm_cpid = %d \n", getpid(), buf.shm_cpid);
        printf("chlid pid=%d, shm_lpid = %d \n", pid, buf.shm_lpid);
		printf("mode = %08x \n", buf.shm_perm.mode);

		addr = (char*)shmat(shmid, NULL, 0);
		if ((int)addr == -1)
		{
			perror("shmat addr error");
			return -1;
		}
	
		printf("%s", addr);
		shmdt(addr);
		shmctl(shmid, IPC_RMID, NULL);
	}	
	else 
	{
		perror("fork error");
		shmctl(shmid, IPC_RMID, NULL);
		return -1;
	}

	return 0;
}











