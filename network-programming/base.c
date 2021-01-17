//2021年1月12日07点32分

#include "base.h"

void sys_err(const char *str)
{
    perror(str);
    exit(1);
}

int Socket(int domain, int type, int protocol)
{
    int sockfd = socket(domain, type, protocol);
    if(sockfd == -1)
    {
        sys_err("socket() error");
    }
    return sockfd;
}

int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int ret = bind(sockfd, addr, addrlen);
    if(ret == -1)
    {
        sys_err("bind() error");
    }
    return ret;
}

int Listen(int sockfd, int backlog)
{
    int ret = listen(sockfd, backlog);
    if(ret == -1)
    {
        sys_err("listen() error");
    }
    return ret;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int clnt_sock = accept(sockfd, addr, addrlen);
    if(clnt_sock == -1)
    {
        sys_err("accept() error");
    }
    return clnt_sock;
}

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int ret = connect(sockfd, addr, addrlen);
    if(ret == -1)
    {
        sys_err("connect() error");
    }
    return ret;
}