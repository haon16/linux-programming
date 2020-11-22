//fcntl实现dup

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fd = open(argv[1], O_RDWR);
    printf("fd = %d\n", fd);

    int newfd1 = fcntl(fd, F_DUPFD, 0);     //0被占用，fcntl使用文件描述符表中可用的最小文件描述符返回
    printf("newfd1 = %d\n", newfd1);

    int newfd2 = fcntl(fd, F_DUPFD, 7);     //7未被占用，返回>=7的文件描述符
    printf("newfd2 = %d\n", newfd2);

    int ret = write(newfd2, "YYYYYYY", 7);
    printf("ret = %d\n", ret);

    return 0;
}