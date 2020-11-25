//回收多个子进程

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    int i;
    pid_t wpid, temp;
    for(i = 0; i < 5; i++)
    {
        temp = fork();
        if(temp == 0)
            break;
    }

    if(i == 5)
    {
        /*
        while((wpid = waitpid(-1, NULL, 0)))        //使用阻塞方式回收子进程
        {
            printf("wait child pid = %d\n", wpid);
        }
        */

        while((wpid = waitpid(-1, NULL, WNOHANG)) != -1)        //使用非阻塞方式回收子进程
        {
            if(wpid > 0)
            {
                printf("wait child pid = %d\n", wpid);
            }
            else if(wpid == 0)
            {
                //sleep(1);
            }
            
        }
    }
    else
    {
        sleep(i);
        printf("I'm %dth child, pid = %d\n", i+1, getpid());
    }

    return 0;
}