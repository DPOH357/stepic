#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
	sem_t* sem;

	sem = sem_open("/test.sem", O_CREAT, 0777, 66);

	//sem_close(sem);
	
	//sem_unlink("/test.sem");

	return 0;
}
