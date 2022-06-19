#include "wrap.h"
void perr_exit(const char* s){
    perror(s);
    exit(-1);
}
int Socket(int family, int type, int protocol){
    int n;
    if((n=socket(family, type, protocol))<0)
        perr_exit("socket error");
    return n;
}
int Connect(int fd, const struct sockaddr* sa, socklen_t salen){
    int n;
    n = connect(fd, sa, salen);
    if(n<0)
        perr_exit("connect error");
    return n;
}
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr){
    int n;
    do{
        n = accept(fd, sa, salenptr);
        if(n>=0)
            return n;
    }while(errno==ECONNABORTED || errno==EINTR);
    perr_exit("accept error");
    return -1;
}
int Bind(int fd, const struct sockaddr *sa, socklen_t salen){
    int n;
    if((n = bind(fd, sa, salen))<0)
        perr_exit("bind error");
    return n;
}
int Listen(int fd, int backlog){
    int n;
    if((n = listen(fd,backlog))<0)
        perr_exit("listen error");
    return n;
}
int Close(int fd){
    int n;
    if((n=close(fd))<0)
        perr_exit("close error");
    return n;
}
ssize_t Read(int fd, char *ptr, size_t nbyte){
    ssize_t n;
    do{
        n = read(fd, ptr, nbyte);
        if(n>=0)
            return n;
    }while(errno==EINTR);
    perr_exit("read error");
    return -1;
}
ssize_t Write(int fd, const void* ptr, size_t nbytes){
    ssize_t n;
    do{
        n = write(fd, ptr, nbytes);
        if(n>=0)
            return n;
    }while(errno==EINTR);
    perr_exit("write error");
    return -1;
}
int Epoll_create(int size){
    int efd = epoll_create(size);
    if(0>efd)
        perr_exit("epoll_create error");
    return efd;
}
int Epoll_ctl(int epfd, int op, int fd, struct epoll_event *event){
    int res = epoll_ctl(epfd, op, fd, event);
    if(0>res)
        perr_exit("epoll_ctl error");
    return res;
}
int Epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout){
    int nready = epoll_wait(epfd, events, maxevents, timeout);
    if(0>nready)
        perr_exit("epoll_wait error");
    return nready;
}