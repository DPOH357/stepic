#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int main(int argc, char** argv)
{
	key_t key = ftok("/tmp/sem.temp", 1);

	union semun
	{ 
		int val;
		struct semid_ds* buf;
		unsigned short* array;
	} arg;

	int fd = semget(key, 16, IPC_CREAT | 0666);
	for(int i = 0; i < 16; i++)
	{
		arg.val = i;
		semctl(fd, i, SETVAL, arg);
	}
/*	
	struct sembuf sops[16];

	for(int i = 0; i < 16; i++)
	{
		sops[i].sem_num = i;
		sops[i].sem_op = i;
		sops[i].sem_flg = 0;		
	}

	semop(fd, sops, 16);
*/
	return 0;
}
