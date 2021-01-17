//2021年1月17日19点19分
//多进程并发服务器实现

#define _POSIX_SOURCE   //struct sigaction
#include "../base.h"
#include <string.h>     //memset()
#include <strings.h>    //bzero()
#include <ctype.h>
#include <signal.h>     //struct sigaction
#include <sys/wait.h>   //waitpid()

void catch_child(int signum)
{
    while(waitpid(0, NULL, WNOHANG) > 0);
    return;
}

int main()
{
    int serv_sock = Socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in serv_adr;
    //memset(&serv_adr, 0, sizeof(serv_adr));
    bzero(&serv_adr, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(9999);
    serv_adr.sin_family = htonl(INADDR_ANY);
    Bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));

    Listen(serv_sock, 128);

    int clnt_sock;
    struct sockaddr_in clnt_adr;
    socklen_t clnt_adr_len = sizeof(clnt_adr);
    pid_t pid;
    int ret;
    int flag = 0;
    printf("parent process pid = %d\n", getpid());
    while (1)
    {
        clnt_adr_len = sizeof(clnt_adr);
        clnt_sock = Accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_len);

        pid = fork();
        if(pid < 0)
        {
            sys_err("fork() error");
        }
        else if(pid == 0)
        {
            Close(serv_sock);
            break;
        }
        else
        {
            printf("accept() ok, child process pid = %d\n", pid);

            //注册信号，用于回收子进程
            if(!flag)
            {
                struct sigaction act;
                act.sa_flags = 0;
                act.sa_handler = catch_child;
                sigemptyset(&act.sa_mask);
                ret = sigaction(SIGCHLD, &act, NULL);
                if(ret == -1)
                {
                    sys_err("sigaction() error");
                }
                flag = 1;
            }
            
            Close(clnt_sock);
        }
    }
    
    char buf[BUFSIZ];
    if(pid == 0)    //子进程逻辑
    {
        while(1)
        {
            ret = Read(clnt_sock, buf, sizeof(buf));
            if(ret == 0)
            {
                Close(clnt_sock);
                exit(1);
            }

            for(int i = 0; i < ret; i++)
            {
                buf[i] = toupper(buf[i]);
            }
            Write(clnt_sock, buf, ret);
            Write(STDOUT_FILENO, buf, ret);
        }
    }


    return 0;
}
