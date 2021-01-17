//2021年1月15日08点09分
//进行错误处理函数封装后的client

#include "../base.h"

int main(int argc, char *argv[])
{
    int clnt_sock = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9999);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr.s_addr);
    Connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    int ret;
    int counter = 10;
    char buf[BUFSIZ];
    while(counter--)
    {
        write(clnt_sock, "hello\n", 6);
        ret = read(clnt_sock, buf, sizeof(buf));
        write(STDOUT_FILENO, buf, ret);
        sleep(1);
    }

    close(clnt_sock);
    return 0;
}