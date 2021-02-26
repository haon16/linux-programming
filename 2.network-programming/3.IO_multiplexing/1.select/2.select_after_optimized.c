//2021年2月16日12点27分
//select实现多路IO转接：添加一个自定义数组提高效率

#include "../../base.h"
#include <strings.h>
#include <ctype.h>
#include <sys/select.h>

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

    Bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr));

    Listen(serv_sock, 128);

    int maxfd = serv_sock;          //最大文件描述符

    int maxi = -1;                  //下标
    int clnt[FD_SETSIZE];           //FD_SETSIZE = 1024
    for(int i = 0; i < FD_SETSIZE; i++)
    {
        clnt[i] = -1;
    }

    fd_set rset, allset;            
    FD_ZERO(&allset);              
    FD_SET(serv_sock, &allset);  
     
    int clnt_sock;
    struct sockaddr_in clnt_adr;
    socklen_t clnt_adr_len;
    char buf[BUFSIZ], str[INET_ADDRSTRLEN];     //INET_ADDRSTRLEN = 16
    int ret, n, i, sockfd;
    while(1)
    {
        rset = allset;                          //每次循环时都重新设置select监控信号集
        ret = select(maxfd+1, &rset, NULL, NULL, NULL);     
        if (ret < 0)
        {
            sys_err("select() error");
        }

        if(FD_ISSET(serv_sock, &rset))                      
        {
            clnt_adr_len = sizeof(clnt_adr);
            clnt_sock = Accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_len);
            printf("received from %s at PORT %d\n", inet_ntop(AF_INET, &clnt_adr.sin_addr, str, sizeof(str)), ntohs(clnt_adr.sin_port));

            for(i = 0; i < FD_SETSIZE; i++)             //找出clnt[]中没有使用的位置，保存accept返回的文件描述符
            {
                if(clnt[i] < 0)
                {
                    clnt[i] = clnt_sock;            
                    break;
                }
            }
            if(i == FD_SETSIZE)                         //达到select能监控的文件个数上限1024
            {
                fputs("too many clients\n", stderr);
                exit(1);
            }

            FD_SET(clnt_sock, &allset);                     
            
            if(maxfd < clnt_sock)       
                maxfd = clnt_sock;
            
            if(i > maxi)                    //保证maxi存的总是client[]最后一个元素下标
                maxi = i;

            if(--ret == 0)                       
                continue;
        }

        for(i = 0; i <= maxi; i++)          //检测哪个client有数据就绪
        {
            if((sockfd = clnt[i]) < 0)
            {
                continue;
            }
            if(FD_ISSET(sockfd, &rset))
            {
                n = Read(sockfd, buf, sizeof(buf));
                if(n == 0)     
                {
                    Close(sockfd);
                    FD_CLR(sockfd, &allset);
                    clnt[i] = -1;           
                }
                else
                {
                    for(int j = 0; j < n; j++)
                    {
                        buf[j] = toupper(buf[j]);
                    }
                    Write(sockfd, buf, n);
                    Write(STDOUT_FILENO, buf, n);
                }

                if(--ret == 0)
                    break;          //跳出for，但还在while中
            }
        } 
    }

    Close(serv_sock);
    return 0;
}