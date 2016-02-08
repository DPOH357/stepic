#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char** argv)
{
	key_t key = ftok("/tmp/mem.temp", 1);

	int id = shmget(key, 1048576, IPC_CREAT | 0660);

	char* ptr = (char*) shmat(id, 0, 0);

	for(int i = 0; i < 1048576; i++)
	{
		ptr[i] = 42;
	}

	return 0;
}
