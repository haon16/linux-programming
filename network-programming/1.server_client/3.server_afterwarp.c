//2021年1月15日08点07分
//进行错误处理函数封装后的server

#include "../base.h"

int main(int argc, char *argv[])
{
    int serv_sock = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9999);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    
    Listen(serv_sock, 128);

    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    int clnt_sock = Accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_len);

    char clnt_IP[1024];
    printf("client ip:%s, port:%d\n", inet_ntop(AF_INET, &clnt_addr.sin_addr.s_addr, clnt_IP, sizeof(clnt_IP)), ntohs(clnt_addr.sin_port));

    char buf[BUFSIZ];
    while(1)
    {
        int ret = read(clnt_sock, buf, sizeof(buf));
        if(ret == 0)
        {
            break;
        }
        write(STDOUT_FILENO, buf, ret);

        for(int i = 0; i < ret; i++)
        {
            buf[i] = toupper(buf[i]);
        }
        write(clnt_sock, buf, ret);
    }

    close(serv_sock);
    close(clnt_sock);
    return 0;
}