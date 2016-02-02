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

    socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
    printf("Crated sockets pair [%d-%d]\n", fd[0], fd[1]);

    if(fork())
    {
        // parent
        read(fd[1], buffer, buff_size);
        write(fd[0], buffer, buff_size);
    }
    else
    {
        // child
        read(fd[0], buffer, buff_size);
        write(fd[1], buffer, buff_size);
    }

    return 0;
}

