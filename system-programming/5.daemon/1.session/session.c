//会话session

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    pid_t pid;
    if((pid = fork()) < 0)
    {
        perror("fork error");
        exit(1);
    }
    else if(pid == 0)
    {
        sleep(5);
        printf("Child process PID is %d\n", getpid());
        printf("Group ID of child is %d\n", getpgid(0));
        printf("Session ID of child is %d\n", getsid(0));

        sleep(10);
        setsid();           //子进程非组长进程，故其成为新会话首进程，且成为组长进程。该进程组id即为会话进程id

        printf("Changed:\n");

        printf("Child process PID is %d\n", getpid());
        printf("Group ID of child is %d\n", getpgid(0));
        printf("Session ID of child is %d\n", getsid(0));

        sleep(10);
    }

    sleep(5);       //一开始主进程和子进程会是同一个终端，子进程创建会话后变成无终端。会话应用：后台服务进程
    return 0;
}