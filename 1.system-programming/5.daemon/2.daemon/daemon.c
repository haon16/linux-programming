//守护进程创建

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

void sys_err(const char *str)
{
    perror(str);
    exit(1);
}

int main()
{
    pid_t pid = fork();
    if(pid < 0)
        sys_err("fork error");
    else if(pid == 0)
    {
        pid = setsid();                 //创建新会话
        if(pid == -1)
            sys_err("setsid error");
        
        int ret = chdir("/root");       //改变工作目录位置
        if(ret == -1)
            sys_err("chdir error");

        umask(0022);                    //改变文件访问权限掩码

        close(STDIN_FILENO);            //关闭文件描述符0 stdin
        int fd = open("/dev/null", O_RDWR);
        if(fd == -1)
            sys_err("open error");
        dup2(fd, STDOUT_FILENO);        //重定向 stdout 和 stderr
        dup2(fd, STDERR_FILENO);

        while(1);                       //模拟守护进程业务
    }


    return 0;           //父进程终止
}