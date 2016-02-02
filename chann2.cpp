#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char ** argv)
{
    const char name_fifo_in[] = "in.fifo";
    const char name_fifo_out[] = "out.fifo";
    const unsigned int buff_max_size = 64;

    if(mkfifo(name_fifo_in, 0666))
    {
        return 1;
    }
    printf("fifo_in created\n");

    if(mkfifo(name_fifo_out, 0666))
    {
        return 2;
    }
    printf("fifo_out created\n");

    int fd_in = open(name_fifo_in, O_RDONLY);
    if(fd_in <= 0)
    {
        return 3;
    }
    printf("fifo_in opened\n");

    int fd_out = open(name_fifo_out, O_WRONLY);
    if(fd_out <= 0)
    {
        return 4;
    }
    printf("fifo_out opened\n");

    char buffer[buff_max_size];
    int len;
    while(1)
    {
        memset(buffer, 0, buff_max_size);
        len = read(fd_in, buffer, buff_max_size-1);

        if(len <= 0)
        {
            close(fd_in);
            close(fd_out);
            return 5;
        }

        printf("Readed: %s", buffer);
        write(fd_out, buffer, len);
    }

    return 0;
}
