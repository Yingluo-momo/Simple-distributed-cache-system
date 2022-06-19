#include<netinet/in.h>   // sockaddr_in
#include<sys/types.h>    // socket
#include<sys/socket.h>   // socketr
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<iostream>
#include<string>
#include<cstdlib>
#include<cstdio>
#include<map>
#include<cstring>
using namespace std;
#define BUFFER_SIZE 1024

enum Type {HEART, OTHER};

struct PACKET_HEAD
{
    Type type;
    int length;
};

class Client
{
private:
    struct sockaddr_in server_addr;
    socklen_t server_addr_len;
    map<string, string> m;
    int fd;
public:
    Client(string ip, int port);
    ~Client();
    void Connect();
    void Run();

};

Client::Client(string ip, int port)
{
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) == 0)
    {
        cout << "Server IP Address Error!";
        exit(1);
    }
    server_addr.sin_port = htons(port);
    server_addr_len = sizeof(server_addr);
    // create socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        cout << "Create Socket Failed!";
        exit(1);
    }
}

Client::~Client()
{
    close(fd);
}

void Client::Connect()
{
    cout << "Connecting......" << endl;
    if(connect(fd, (struct sockaddr*)&server_addr, server_addr_len) < 0)
    {
        cout << "Can not Connect to Server IP!";
        exit(1);
    }
    cout << "Connect to Server successfully." << endl;
}

void Client::Run()
{
    while(1){
        PACKET_HEAD head;
        head.type = OTHER;
        head.length = 3;
        string key, value;
        cout<<"please input key:"<<endl;
        cin>>key;
        cout<<"please input value:"<<endl;
        cin>>value;
        m.insert({key, value});
        send(fd, &m, sizeof(m), 0);
    }
}


int main()
{
    Client client("192.168.56.1", 14999);
    client.Connect();
    client.Run();
    while(1)
    {
        string msg;
        getline(cin, msg);
        if(msg == "exit")
            break;
        cout << "msg\n";
    }
    return 0;
}
