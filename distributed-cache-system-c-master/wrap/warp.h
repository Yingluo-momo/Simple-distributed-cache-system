#ifndef WRAP_H
#define WRAP_H
#include "iostream"
#include "unistd.h"
#include "arpa/inet.h"
#include "sys/epoll.h"
#include "cerrno"

void perr_exit(const char* s);
int Socket(int family, int type, int protocol);
int Connect(int fd, const struct sockaddr* sa, socklen_t salen);
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
int Bind(int fd, const struct sockaddr *sa, socklen_t salen);
int Listen(int fd, int backlog);
int Close(int fd);
ssize_t Read(int fd, char *ptr, size_t nbyte);
ssize_t Write(int fd, const void* ptr, size_t nbytes);
int Epoll_create(int size);
int Epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int Epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
#endif /* WRAP_H */
