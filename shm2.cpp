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

	ftruncate(fd, 1048576);

	ptr = (char*) mmap(0, 1048576, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

	for(i = 0; i < 1048576; i++)
	{
		ptr[i] = 13;
	}

	//munmap(ptr, 1024);

	//shm_unlink("/test.shm");

	return 0;
}
