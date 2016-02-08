#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char** argv)
{
	key_t key = ftok("/tmp/mem.temp", 1);

	int id = shmget(key, 2000000, IPC_CREAT | 0666);

	char* ptr = (char*) shmat(id, 0, 0);

	for(unsigned int i = 0; i < 2000000; i++)
	{
		ptr[i] = 42;
	}

	return 0;
}
