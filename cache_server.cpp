//心跳包程序直接写到client_Run()里面
//开两个线程，一个用作server，一个只发心跳包

#include<netinet/in.h>   // sockaddr_in
#include<sys/types.h>    // socket
#include<sys/socket.h>   // socket
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/select.h>   // select
#include<sys/ioctl.h>
#include<sys/time.h>
#include<iostream>
#include<vector>
#include<map>
#include<string>
#include<cstdlib>
#include<cstdio>
#include<cstring>
#include<thread>
#include<map>

//#include"cache_server_client.h"
using namespace std;
#define BUFFER_SIZE 1024

enum Type {HEART, OTHER};

struct PACKET_HEAD
{
    Type type;
    int length;
};

class Server
{
private:
    //作为服务器的ip和地址
    struct sockaddr_in server_addr_server;
    socklen_t server_addr_len_server;

    //作为客户端的ip和地址
    struct sockaddr_in server_addr_client;
    socklen_t server_addr_len_client;

    int listen_fd;    // 监听的fd
    int max_fd;       // 最大的fd

    int fd;  //作为客户端的fd
    fd_set master_set;   // 所有fd集合，包括监听fd和客户端fd
    fd_set working_set;  // 工作集合
    struct timeval timeout;
    map<int, pair<string, int> > mmap;   // 记录连接的客户端fd--><ip, count>
public:
    Server(int server_port, string master_ip, int master_port);
    ~Server();
    void Bind();
    void Listen(int queue_len = 20);
    void Accept();
    void server_Run();
    void Recv(int nums);

    //作为master的客户端
    void Connect();
    void client_Run();
};


Server::Server(int server_port, string master_ip, int master_port)
{
//server
    bzero(&server_addr_server, sizeof(server_addr_server));
    server_addr_server.sin_family = AF_INET;
    server_addr_server.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr_server.sin_port = htons(server_port);
    // create socket to listen
    listen_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(listen_fd < 0)
    {
        cout << "Create Socket Failed!";
        exit(1);
    }
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

//client
    bzero(&server_addr_client, sizeof(server_addr_client));
    server_addr_client.sin_family = AF_INET;
    if(inet_pton(AF_INET, master_ip.c_str(), &server_addr_client.sin_addr) == 0)
    {
        cout << "Server IP Address Error!";
        exit(1);
    }
    server_addr_client.sin_port = htons(master_port);
    server_addr_len_client = sizeof(server_addr_client);
    // create socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        cout << "Create Socket Failed!";
        exit(1);
    }

}


Server::~Server()
{
    for(int server_fd=0; server_fd<=max_fd; ++fd)
    {
        if(FD_ISSET(server_fd, &master_set))
        {
            close(server_fd);
        }
    }

    close(fd);
}

void Server::Bind()
{
    if(-1 == (bind(listen_fd, (struct sockaddr*)&server_addr_server, sizeof(server_addr_server))))
    {
        cout << "Server Bind Failed!";
        exit(1);
    }
    cout << "Bind Successfully.\n";
}

void Server::Listen(int queue_len)
{
    if(-1 == listen(listen_fd, queue_len))
    {
        cout << "Server Listen Failed!";
        exit(1);
    }
    cout << "Listen Successfully.\n";
}

void Server::Accept()
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int new_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if(new_fd < 0)
    {
        cout << "Server Accept Failed!";
        exit(1);
    }

    string ip(inet_ntoa(client_addr.sin_addr));    // 获取客户端IP

    cout << ip << " new connection was accepted.\n";

    mmap.insert(make_pair(new_fd, make_pair(ip, 0)));

    // 将新建立的连接的fd加入master_set
    FD_SET(new_fd, &master_set);
    if(new_fd > max_fd)
    {
        max_fd = new_fd;
    }
}

void Server::Recv(int nums)
{
    for(int server_fd=listen_fd+1; server_fd<=max_fd; ++server_fd)
    {
        if(FD_ISSET(server_fd, &working_set)) //如果fd在woring_set中，则进行接收数据
        {
            int n;  // 标记当前连接是否断开了

            PACKET_HEAD head;
            n = recv(server_fd, &head, sizeof(head), 0);   // 先接受包头

            if(head.type == OTHER)
            {
                //接收数据，功能未添加
                //cout << "Received data from client.\n";

            }
            else
            {
                // 数据包，通过head.length确认数据包长度
            }


            if(!n)  // 当前这个连接有问题，关闭它
            {
                close(server_fd);
                FD_CLR(server_fd, &master_set);

                if(server_fd == max_fd)  // 需要更新max_fd;
                {
                    while(FD_ISSET(max_fd, &master_set) == false)
                        --max_fd;
                }
            }
        }
    }
}

void Server::server_Run()
{

    Bind();  //io检测
    Listen();  //io检测

    max_fd = listen_fd;   // 初始化max_fd
    FD_ZERO(&master_set);
    FD_SET(listen_fd, &master_set);  // 添加监听fd

    while(1)
    {
        FD_ZERO(&working_set);
        memcpy(&working_set, &master_set, sizeof(master_set));


        int nums = select(max_fd+1, &working_set, NULL, NULL, NULL);
        if(nums < 0)
        {
            cout << "select() error!";
            exit(1);
        }

        if(nums == 0)
        {
            //cout << "select() is timeout!";
            continue;
        }

        if(FD_ISSET(listen_fd, &working_set))
            Accept();   // 有新的客户端请求
        else
            Recv(nums); // 接收客户端的消息
    }
}

void Server::Connect()
{
    cout << "Connecting......" << endl;
    if(connect(fd, (struct sockaddr*)&server_addr_client, server_addr_len_client) < 0)
    {
        cout << "Can not Connect to Server IP!";
        exit(1);
    }
    cout << "Connect to Server successfully." << endl;
}

void Server::client_Run()
{
    Connect();
    cout << "The heartbeat sending thread started.\n";
    int count = 0;  // 测试
    while(1)
    {
        PACKET_HEAD head;
        head.type = HEART;
        head.length = 0;
        send(fd, &head, sizeof(head), 0);  //send函数发送心跳
        sleep(3);     // 定时3秒

        ++count;      // 测试：发送15次心跳包就停止发送
        if(count > 15)
            break;
    }
}


int main()
{
    Server cache_server1(14999, "192.168.56.1", 15000); //客户端连接的端口为14999，master_ip "192.168.1.7"，端口15000
    Server cache_server2(14998, "192.168.56.1", 15000); //cache_server2
    Server cache_server3(14997, "192.168.56.1", 15000); //cache_server3

    thread th[6];
    th[0] = thread(&Server::server_Run, &cache_server1);
    th[1] = thread(&Server::client_Run, &cache_server1);

    th[2] = thread(&Server::server_Run, &cache_server2);
    th[3] = thread(&Server::client_Run, &cache_server2);

    th[4] = thread(&Server::server_Run, &cache_server3);
    th[5] = thread(&Server::client_Run, &cache_server3);

    th[0].join();
    th[1].join();
    th[2].join();
    th[3].join();
    th[4].join();
    th[5].join();

    return 0;
}
