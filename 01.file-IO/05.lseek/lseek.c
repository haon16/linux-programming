//lseek函数

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, 0644);
    if(fd < 0)
    {
        perror("open error");
        exit(1);
    }

    char msg[] = "It's a test for lseek\n";
    write(fd, msg, strlen(msg));    //使用fd对打开的文件进行写操作，之后读写位置位于文件结尾处

    //lseek(fd, 0, SEEK_SET);       //修改文件读写位置指针，位于文件开头

    int n;
    char ch;
    while(n = read(fd, &ch, 1))
    {
        if(n < 0)
        {
            perror("read error");
            exit(1);
        }
        write(STDOUT_FILENO, &ch, n);
    }

    close(fd);
    return 0;
}