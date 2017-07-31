#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

typedef struct
{
	char    name[8];
	int     age;
}test_t;

int main()
{
	int      shmid;
	int      i;
	char     temp[8];
	char     pathname[30];
	key_t    key;
	test_t  *t_map;

	memset(temp, 0, sizeof(temp));

	strcpy(pathname, "/tmp");

	key = ftok(pathname, 0x07);

	if (key == -1)
	{
		perror("ftok error");
		return -1;
	}
	
	printf("key=%d\n", key);

	shmid = shmget(key, 4096, IPC_CREATE|IPC_EXEC|0600);
	if (shm_id == -1)
	{
		perror("shmget error");
		return -1;
	}

	t_map = (test_t*)shmat(shmid, NULL, 0);
	if ((int)t_map == -1)
	{
		perror("shmat error");
		return -1;
	}

	strcpy(temp, "test");
	temp[4] = '0';
	for (i=0; i<3; ++i)
	{
		temp[4] += 1;
		strncpy((t_map + i)->name, temp, 5);
		(t_map + i)->age = 0 + i;
	}

	shmdt(t_map);

	shmctl(shmid, IPC_RMID, NULL);//ipcrm -m shmid
	
	return 0;
}








