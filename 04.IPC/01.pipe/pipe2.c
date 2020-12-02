//pipe测试

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void sys_error(char *str)
{
    perror(str);
    exit(1);
}

int main()
{
    int fd[2];
    int ret = pipe(fd);
    if(ret == -1)
        sys_error("pipe error");
    
    char str[] = "hello world\n";
    char buf[100];
    int pid = fork();
    if(pid == 0)
    {
        close(fd[1]);
        int n = read(fd[0], buf, sizeof(buf));
        printf("child read n = %d\n", n);
        printf("%s", buf);
        close(fd[0]);
    }
    else if (pid > 0)
    {
        close(fd[0]);

        int n = write(fd[1], str, strlen(str));
        sleep(1);               //延迟父进程结束
        close(fd[1]);

        /*
        sleep(3);               //测试读管道时，有写端但无数据的情况，阻塞等待
        int n = write(fd[1], str, strlen(str));
        close(fd[1]);
        */

        /*
        sleep(3);
        close(fd[1]);           //不写入数据直接关闭写端，读端会返回0
        */
    }
    
    return 0;
}