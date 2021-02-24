//2021年1月12日07点10分
//对基本函数进行错误处理封装

#ifndef _BASE_H
#define _BASE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>      // struct sockaddr_in
#include <sys/socket.h>

void sys_err(const char *str);
int Socket(int domain, int type, int protocol);
int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
ssize_t Read(int fd, void *buf, size_t count);
ssize_t Write(int fd, const void *buf, size_t count);
int Close(int fd);


#endif      /* _BASE_H */