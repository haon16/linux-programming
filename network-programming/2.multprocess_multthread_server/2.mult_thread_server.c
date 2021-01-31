//2021年1月23日22点01分
//多线程并发服务器实现

#include "../base.h"
#include <strings.h>    //bzero()
#include <ctype.h>
#include <pthread.h>

struct clnt_info                //定义一个结构体，将地址结构跟客户端文件描述符绑定（主要是为了演示多线程怎么传复杂数据类型）
{
    struct sockaddr_in clnt_addr;
    int clnt_sock; 
};

void *do_work(void *arg)
{
    struct clnt_info *p_cInfo = (struct clnt_info *)arg;
    char buf[BUFSIZ];
    char str[16];       //sizeof(sockaddr_in)
    int n;

    while(1)
    {
        n = Read(p_cInfo->clnt_sock, buf, sizeof(buf));
        if(n == 0)
        {
            printf("the client %d closed...\n", p_cInfo->clnt_sock);
            break;
        }

        printf("received from %s at PORT %d\n", inet_ntop(AF_INET, &(p_cInfo->clnt_addr.sin_addr), str, sizeof(str)), ntohs(p_cInfo->clnt_addr.sin_port));  //打印客户端信息（IP/PORT）

        for(int i = 0; i < n; i++)
        {
            buf[i] = toupper(buf[i]);
        }

        Write(STDOUT_FILENO, buf, n);
        Write(p_cInfo->clnt_sock, buf, n);
    }
    Close(p_cInfo->clnt_sock);
    return (void*)0;        //pthread_exit(0)
}

int main()
{
    int serv_sock = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9999);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //int opt = 1;     
    //setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&opt, sizeof(opt));
    Bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    Listen(serv_sock, 128);

    printf("Accepting client connect...\n");
    struct clnt_info c_info[256];
    int i = 0;
    int clnt_sock;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len;
    pthread_t tid;
    while(1)
    {
        clnt_addr_len = sizeof(clnt_addr);
        clnt_sock = Accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_len);
        c_info[i].clnt_addr = clnt_addr;
        c_info[i].clnt_sock = clnt_sock;

        pthread_create(&tid, NULL, do_work, (void*)&c_info[i]);     //传地址然后根据首地址强转，如果是传值，该结构体占20个字节，实际上传到回调函数的只有sizeof(void *)个字节，会数据丢失
        pthread_detach(tid);            //子线程分离，防止僵尸线程
        i++;
    }

    return 0;
}