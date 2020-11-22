//dup函数

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fd = open(argv[1], O_RDWR|O_CREAT, 0644);   // 3
    if(fd < 0)
    {
        perror("open error");
        exit(1);
    }

    int newfd = dup(fd);                // 4
    printf("newfd = %d\n", newfd);

    write(newfd, "hello world\n", strlen("hello world\n"));

    close(fd);
    close(newfd);
    return 0;
}