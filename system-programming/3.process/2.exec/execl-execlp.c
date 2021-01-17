//execl函数 ：加载一个进程，通过 路径+程序名 来加载
//execlp函数：加载一个进程，借助PATH环境变量

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    pid_t pid = fork();
    if(pid == -1)
    {
        perror("fork error");
        exit(1);
    }
    else if(pid == 0)
    {
        //错误写法
        //execlp("ls", "-l", "-h", NULL);   //NULL是结束标记，正确执行的话不返回，错误返回-1

        //execlp("ls", "ls", "-l", "-h", NULL);
        //execlp("date", "date", NULL);

        //execl("./test", "test", NULL);
        execl("/bin/ls", "ls", "-l", NULL);

        perror("exec error");
        exit(1);
    }
    else if(pid > 0)
    {
        sleep(1);
        printf("I'm parent : %d\n", getpid());
    }

    return 0;
}