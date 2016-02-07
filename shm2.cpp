#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
	int fd;
	char* ptr;
	int i;

	fd = shm_open("/test.shm", O_CREAT | O_RDWR, 0660);

	ftruncate(fd, 1024);

	ptr = (char*) mmap(0, 1024, PROT_WRITE, MAP_SHARED, fd, 0);

	for(i = 0; i < 1024; i++)
	{
		ptr[i] = 13;
	}

	munmap(ptr, 1024);

	return 0;
}
