//#pragma once
#include "cstring"
#include "cctype"
#include "fcntl.h"
#include "wrap.hpp"
#include "json.hpp"
#include "LRU.hpp"

#define HEARTBEAT_CACHE 3
#define CACHE_NUM 0
#define HEARTBEAT_HEALTHY 0
#define HEARTBEAT_UNHEALTHY 1
#define READ_CLIENT_CACHE 0
#define WRITE_CLIENT_CACHE 1
#define FOUND 0
#define NOTFOUND 1
#define MASTER_IP "127.0.0.1"
#define MASTER_PORT 7000
#define CACHESERV_PORT 8001

using json = nlohmann::json;
using namespace std;
json pack_json_heartbeat(){
    json data, heartbeat_json;
    data["number"] = CACHE_NUM;
    data["state"] = HEARTBEAT_HEALTHY;
    heartbeat_json["type"] = HEARTBEAT_CACHE;
    heartbeat_json["data"] = data;
    return heartbeat_json;
}

void *heart_beat(void *arg){
    json heartbeat_json;
    heartbeat_json = pack_json_heartbeat();
    struct sockaddr_in serv_addr;
    socklen_t serv_addr_len;
    int cfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, MASTER_IP, &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(MASTER_PORT);
    Connect(cfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    string buf = heartbeat_json.dump();
    while(1){
        sleep(1);
        //heartbeat_json["data"]["state"] = 1;
        Write(cfd, (char *)buf.data(), buf.length());
        //cout<<buf<<endl;
    }
}

int main(){
    pthread_t tid_heartbeat;
    Pthread_create(&tid_heartbeat, NULL, heart_beat, NULL);
    LRUCache LC(100);
    int openmax = 100;
    int num = 0, listen_num = 20;
    char buf[BUFSIZ], str[openmax];
    struct sockaddr_in cliaddr, servaddr;
    struct epoll_event tep, ep[openmax];
    int listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(CACHESERV_PORT);
    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    Listen(listenfd, listen_num);
    int efd = Epoll_create(openmax);
    tep.events = EPOLLIN | EPOLLET;
    tep.data.fd = listenfd;
    int res = Epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &tep);
    for(;;){
        int nready = Epoll_wait(efd, ep, openmax, -1);
        for(int i=0;i<nready;i++){
            if(0>nready)
                continue;
            if(ep[i].data.fd==listenfd){
                socklen_t clilen = sizeof(cliaddr);
                int connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
                cout<<"received from "<<\
                inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str))<<\
                "at PORT "<<ntohs(cliaddr.sin_port)<<endl;
                cout<<"cfd "<<connfd<<"---client"<<++num<<endl;

                auto flag = fcntl(connfd, F_GETFL);
                fcntl(connfd, F_SETFL, flag | O_NONBLOCK);
                tep.events = EPOLLIN | EPOLLOUT | EPOLLET;
                tep.data.fd = connfd;
                res = Epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &tep);
            }
            else{
                int sockfd = ep[i].data.fd;
                buf[0] = '\0';
                ssize_t n = Read(sockfd, buf, sizeof(buf));
                if(0==n){
                    res = Epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
                    Close(sockfd);
                    cout<<"client["<<sockfd<<"] closed connection"<<endl;
                }
                else if(0>n){
                    perror("read n<0 error: ");
                    res = Epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
                    Close(sockfd);
                }
                else{
                    buf[n] = '\0';
                    json info = json::parse(buf);

                    if(info["type"]==READ_CLIENT_CACHE){
                        info["data"]["value"] = LC.get(info["data"]["key"]);
                        if(info["data"]["value"].empty())
                            info["data"]["flag"] = NOTFOUND;
                        else
                            info["data"]["flag"] = FOUND;
                        string str_out = info.dump();
                        Write(sockfd, (char *)str_out.data(), str_out.length());
                    }
                    else if(info["type"]==WRITE_CLIENT_CACHE){
                        LC.put(info["data"]["key"], info["data"]["value"]);
                    }
                    /*
                    string temp = info.dump();
                    Write(sockfd, buf, sizeof(buf));*/
                }
            }
        }
    }
    Close(listenfd);
    return 0;
}
