#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

int main(int argc, char** argv)
{
    int fd[2];
    const unsigned int buff_size = 32;
    char buffer[buff_size];

    socketpair(PF_LOCAL, SOCK_STREAM, 0, fd);
    printf("Crated sockets pair [%d-%d]\n", fd[0], fd[1]);

    if(fork())
    {
        // parent
        close(fd[1]);
        fgets(buffer, buff_size, stdin);
        write(fd[0], buffer, buff_size);
    }
    else
    {
        // child
        unsigned int len;

        memset(buffer, 0, buff_size);

        close(fd[0]);
        len = read(fd[1], buffer, buff_size);
        if(len <= 0)
        {
            printf("Read error\n");
        }
        printf("%s\n", buffer);
    }

    return 0;
}

