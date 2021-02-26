//2021年2月13日22点00分
//select实现多路IO转接

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

    fd_set rset, allset;            //定义读集合，备份集合
    FD_ZERO(&allset);               //清空监听集合
    FD_SET(serv_sock, &allset);     //将待监听fd添加到监听集合中
     
    int clnt_sock;
    struct sockaddr_in clnt_adr;
    socklen_t clnt_adr_len;
    char buf[BUFSIZ];
    int ret, n;
    while(1)
    {
        rset = allset;              //备份
        ret = select(maxfd+1, &rset, NULL, NULL, NULL);     //select监听
        if (ret < 0)
        {
            sys_err("select() error");
        }

        if(FD_ISSET(serv_sock, &rset))                      //serv_sock满足监听的读事件
        {
            clnt_adr_len = sizeof(clnt_adr);
            clnt_sock = Accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_len);      //建立连接，不会阻塞
            FD_SET(clnt_sock, &allset);                     //将新产生的fd，添加到监听集合中，监听数据读事件
            
            if(maxfd < clnt_sock)       //修改maxfd
                maxfd = clnt_sock;
            
            if(ret == 1)                //说明select只返回一个并且是serv_sock,后续代码无需执行          
                continue;
        }

        for(int i = serv_sock+1; i <= maxfd; i++)   //处理满足读事件的fd
        {
            if(FD_ISSET(i, &rset))
            {
                n = Read(i, buf, sizeof(buf));
                if(n == 0)
                {
                    Close(i);
                    FD_CLR(i, &allset);             //将关闭的fd移出监听集合
                }
                else
                {
                    for(int j = 0; j < n; j++)
                    {
                        buf[j] = toupper(buf[j]);
                    }
                    Write(i, buf, n);
                    Write(STDOUT_FILENO, buf, n);
                }
            }
        } 
    }

    Close(serv_sock);
    return 0;
}