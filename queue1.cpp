#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MTEXTLEN 80

struct message
{
	long mtype;
	char mtext[MTEXTLEN];
};

int main(int argc, char** argv)
{
	key_t key;
	int id;
	struct message msg;
	int len;
	FILE* file;

	msg.mtype = 0;
	msg.mtext[MTEXTLEN-1] = 0;

	key = ftok("/tmp/msg.temp", 1);
	id = msgget(key, IPC_CREAT | 0666);
	printf("Queue id: %d\n", id);

	len = msgrcv(id, (struct msgbuff*)&msg, sizeof(struct message) - sizeof(long), 0, 0);
	msg.mtext[len] = 0;
	printf("Message length: %d\n", len);

	msgctl(id, IPC_RMID, NULL);

	printf("%s\n", msg.mtext);

	file = fopen("/home/box/message.txt", "w");
	fprintf(file, "%s\n", msg.mtext);
	fclose(file);

	return 0;
}
