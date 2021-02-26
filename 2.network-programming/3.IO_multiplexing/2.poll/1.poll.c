//2021年2月26日21点15分
//poll实现多路IO转接

#include "../../base.h"
#include <ctype.h>
#include <strings.h>
#include <poll.h>

#define MAXLINE 80
#define OPEN_MAX 1024

int main()
{
    int serv_sock = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_adr;
    bzero(&serv_adr, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(9999);
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));

    Listen(serv_sock, 128);

    struct pollfd client[OPEN_MAX];
    client[0].fd = serv_sock;       //要监听的第一个文件描述符存入client[0]
    client[0].events = POLLIN;      //监听普通读事件

    for(int i = 1; i < OPEN_MAX; i++)
    {
        client[i].fd = -1;
    }

    int maxi = 0;           //client数组中有效元素中最大元素下标
    char buf[MAXLINE], str[INET_ADDRSTRLEN];
    struct sockaddr_in clnt_adr;
    socklen_t clnt_adr_len;
    int ret, conn_sock, clnt_sock, i, n;
    while(1)
    {
        ret = poll(client, maxi+1, -1);
        if(ret < 0)
        {
            sys_err("poll() err");
        }

        if(client[0].revents & POLLIN)
        {
            clnt_adr_len = sizeof(clnt_adr);
            conn_sock = Accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_len);
            printf("receive from %s at PORT %d\n", inet_ntop(AF_INET, &clnt_adr.sin_addr, str, sizeof(str)), ntohs(clnt_adr.sin_port));

            for(i = 1; i < OPEN_MAX; i++)
            {
                if(client[i].fd < 0)
                {
                    client[i].fd = conn_sock;           //找到client数组中空闲的位置，存放accept返回的conn_sock
                    break;
                }
            }
            if(i == OPEN_MAX)
            {
                fputs("too many clients\n", stderr);            //达到最大客户端数
                exit(1);
            }
            
            client[i].events = POLLIN;                  //设置刚刚返回的客户端套接字，监控读事件
            if(i > maxi)
                maxi = i;                               //更新client[]中最大元素下标
            if(--ret <= 0)
                continue;                               //没有更多就绪事件时，继续回来poll阻塞
        }

        for(i = 1; i <= maxi; i++)
        {
            if((clnt_sock = client[i].fd) < 0)
                continue;

            if(client[i].revents & POLLIN)
            {
                if((n = Read(clnt_sock, buf, MAXLINE)) < 0)
                {
                    if(errno == ECONNRESET)             //收到RST标志   （封装的Read()中read = -1时直接退出了，不会走到这）
                    {
                        printf("client[%d] aborted connection\n", i);
                        Close(clnt_sock);
                        client[i].fd = -1;              //poll中不监控文件描述符直接置为-1即可，不用想select那样移除
                    }
                    else
                    {
                        sys_err("read() error");
                    }
                }
                else if(n == 0)                         //说明客户端先关闭连接
                {
                    printf("client[%d] closed connection\n", i);
                    Close(clnt_sock);
                    client[i].fd = -1;
                }
                else
                {
                    for(int j = 0; j < n; j++)
                        buf[j] = toupper(buf[j]);
                    Write(clnt_sock, buf, n);
                }

                if(--ret <= 0)
                    break;
            }
        }
    }

    return 0;
}