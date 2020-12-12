//kill

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int val = 100;

void sys_err(const char *str)
{
    perror(str);
    exit(1);
}

int main()
{
    pid_t pid = fork();
    if(pid > 0)
    {
        while(1)
        {
            printf("parent, pid = %d\n", getpid());
            sleep(1);
        }
    }
    else if(pid == 0)
    {
        /*
        sleep(2);
        printf("child pid = %d, ppid = %d\n", getpid(), getppid());         //子进程给父进程发送信号
        kill(getppid(), SIGKILL);
        */

        /*
        sleep(2);
        printf("child pid = %d, ppid = %d\n", getpid(), getppid());         // kill的参数pid<-1时会取|pid|发给对应进程组   （外部命令发送信号 kill -9 -父进程组id）
        while(1);
        */

        sleep(5);
        printf("child pid = %d, ppid = %d\n", getpid(), getppid());         // kill的参数pid=0时发送信号给与调用kill函数进程属于同一进程组的所有进程
        kill(0, SIGKILL);
    }
    
    return 0;
}