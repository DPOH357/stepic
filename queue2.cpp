#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>

#define BUFFSIZE 32 

int main(int argc, char** argv)
{
	mqd_t mqd;
	struct mq_attr attr;
	char buffer[BUFFSIZE];
	int len;
	FILE* file;

	attr.mq_flags = O_CREAT | O_RDONLY;
	attr.mq_maxmsg = 4;
	attr.mq_msgsize = BUFFSIZE;

	mqd = mq_open("/test.mq", O_CREAT | O_RDONLY, 0666, &attr);
	//mqd = mq_open("/test.mq", O_RDONLY);

	printf("mqd: %d\n", mqd);

	len = mq_receive(mqd, buffer, BUFFSIZE + 1, NULL);
	buffer[len] = 0;

	printf("Received message (%d): %s\n", len, buffer);

	file = fopen("/home/box/message.txt", "w");

	fprintf(file, "%s\n", buffer);

	fclose(file);
	

	mq_close(mqd);
	return 0;
}
