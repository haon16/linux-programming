//借助信号捕捉回收子进程

#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>


void sys_err(const char* str)
{
    perror(str);
    exit(1);
}

void catch_child(int signo)
{
    pid_t wpid;
    int status;

    //if((wpid = wait(NULL)) != -1)                  //存在可能：回收某一子进程时，别的子进程也终止了，此时终止的信号发过来无法处理，导致另外的子进程无法成功回收继而成为僵尸进程，所以不能用if
    while((wpid = waitpid(-1, &status, 0)) != -1)      //循环回收，防止僵尸进程出现       （wait,waitpid 阻塞方式或者非阻塞方式？）
    {
        if(WIFEXITED(status))
            printf("catch child, pid = %d, ret = %d\n", wpid, WEXITSTATUS(status));
    }

    //测试回调函数卡着的情况，父进程会不会继续运行（输出）
    //printf("wpid = %d, error = %d, strerr = %s\n", wpid, errno, strerror(errno));    //子进程全部回收后wait，waitpid会因无子进程可回收而报错，退出循环，如果设置的是阻塞回收，且子进程未结束那么回调函数会卡着等待回收，
    //while(1);                                                                        //导致回调函数无法执行系统调用sigreturn再次进入内核，也就无法返回主控制流程从中断的地方继续向下执行，所以上面要用非阻塞回收
    return;
}

int main()
{
    //设置阻塞（防止子进程在父进程注册信号捕捉函数前结束）
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_BLOCK, &set, NULL);

    pid_t pid;
    int i;
    for(i = 0; i < 10; i++)
    {
        if((pid =fork()) == 0)          //创建多个子进程
            break;
    }

    if(i == 10)
    {
        struct sigaction act;               
        act.sa_handler = catch_child;       //设置回调函数
        sigemptyset(&act.sa_mask);          //设置捕捉函数执行期间屏蔽字
        act.sa_flags = 0;                   //设置默认属性，本信号自动屏蔽
        sigaction(SIGCHLD, &act, NULL);     //注册信号捕捉函数

        //解除阻塞
        sigprocmask(SIG_UNBLOCK, &set, NULL);   //如果没有解除阻塞步骤，回调函数没有执行机会！！！

        printf("I'm parent, pid = %d\n", getpid());
        
        int num = 0;
        while(1)
        {
            printf("num = %d\n", num++);
            sleep(1);
        }
    }
    else
    {
        printf("I'm child, pid = %d\n", getpid());
        return i;
    }

    return 0;
}