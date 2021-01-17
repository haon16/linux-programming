//获取、拓展文件的大小

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fd = open(argv[1], O_RDWR);
    if(fd < 0)
    {
        perror("open error");
        exit(1);
    }

    int length = lseek(fd, 0, SEEK_END);
    printf("file size:%d\n", length);

    lseek(fd, 99, SEEK_END);
    write(fd, "\0", 1);
    length = lseek(fd, 0, SEEK_END);
    printf("file size after expand:%d\n", length);

    close(fd);
    return 0;    
}