#include <iostream>
#include "cstring"
#include "cctype"
#include "fcntl.h"
#include "wrap.hpp"
#include "json.hpp"
#include "thrd_pool.hpp"
#include <pthread.h>
//#include <pthreadtypes.h>
#include "signal.h"
#include <algorithm>
//#include "stl_algo.h"
#include <vector>
using json = nlohmann::json;
//常数定义
#define HEART_BEAT 2
#define DISTRIBUTION_REQUEST 3
#define DISTRIBUTION_RESPOND 4
#define SHUTDOWN_CACHE 6
#define ADD_CACHE 7
#define REFLESH_IP 10//探测到新的cache server心跳包，master向所有cache sever(包括新加入的cache)
int flag1=0;//如果有新的IP加入，就把标志位置1,master向所有cache sever(包括新加入的cache)发送
int flag2=0;//主动缩容，就把标志位置1,Master向shutdown cache发送
int flag3=0;//主动缩容，就把标志位置1,Master向其他cache 发送
pthread_mutex_t task_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_rwlock_t rw_lock = PTHREAD_RWLOCK_INITIALIZER;
//存储加入的client IP\port\IP:port列表
vector<string> C_IPportlist;//IP:port
vector<string> C_IPlist;//IP
vector<int> C_Portlist;//port
//存储加入的cache IP\port\IP:port列表
vector<string> IPportlist;//IP:port
vector<string> IPlist;//IP
vector<int> Portlist;//port
//存放缩容时需要关闭的ip和port
string shutip,shutport;
//存储心跳包里面的ip和port
string heartip,heartport;
//port-0/1，发送后为1，否则为0，以判断是否为所有的cache都发送了新的port
unordered_map<string, int> cachemap;//扩容
unordered_map<string, int> cachemapshut;//缩容,否则置1出现问题
//port-0/1，发送后为1，否则为0，以判断是否为所有的client都发送了新的port
unordered_map<string, int> clientmap;
//扩容发送计数
int num1=0;
//缩容发送计数
int num2=0;

struct pack{
    int sockfd;
    int efd;
    char* buf;
};
class master_handler
{
public:
    //IP分布信息
    void Distributionresponse(int clie_fd);
    //不同节点心跳包的处理
    void Heartbeathandler(int clie_fd, char * buf);
    //关闭节点
    void Shotdown(int clie_fd, char * buf);
    //主动扩容，新加入cache
    void Cachejoin(int clie_fd, char * buf);
};

//IP分布信息
void master_handler::Distributionresponse(int clie_fd)
{
    pthread_rwlock_rdlock(&rw_lock);
    json data,listip;
    vector<string>  dataip;
    //////////////////test/////////
    /*IPportlist.push_back("127.0.0.2:9001");
    IPportlist.push_back("127.0.0.2:9002");
    IPportlist.push_back("127.0.0.2:9003");*/
    ///////////////////////////////
    int size=IPportlist.size();
    listip["type"]=DISTRIBUTION_RESPOND;
    for(int i=0;i<size;i++)
    {
        dataip.push_back(IPportlist[i]);
    }
    data["iplist"]=dataip;
    listip["data"]=data;
    std::string str_out = listip.dump();
    //clie_fd,通信的文件描述符，accept () 函数的返回值
    //(char *)str_out.data(): 传入参数，要发送的字符串
    Write(clie_fd, (char *)str_out.data(), str_out.length());
    Close(clie_fd);
    pthread_rwlock_unlock(&rw_lock);
}
//不同节点心跳包的处理
void master_handler::Heartbeathandler(int clie_fd, char * buf)
{
    
    pthread_rwlock_unlock(&rw_lock);
    //
    //处理心跳包
    //如果某个节点长时间没发，则缩容
    pthread_rwlock_unlock(&rw_lock);
}

void refreship(int clie_fd)
{
    pthread_rwlock_rdlock(&rw_lock);
    //for(int i=0;i<IPlist.size();i++)
	//{
		/*struct sockaddr_in serv_addr;
		socklen_t serv_addr_len;
        
        string ipnow=IPlist[i];
        int portnow = Portlist[i];

		signal(SIGPIPE, SIG_IGN);
		int cfd = Socket(AF_INET, SOCK_STREAM, 0);
		bzero(&serv_addr,sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		inet_pton(AF_INET, ipnow.c_str(), &serv_addr.sin_addr.s_addr);
		serv_addr.sin_port = htons(portnow);
		connect(cfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));//here cannot use Connect
		*/
    //}
    json sum,data;
    if(flag1==1){
        sum["type"]=ADD_CACHE;
    }
    else if(flag2==1){
        sum["type"]=SHUTDOWN_CACHE;
    }
    else if(flag3==1){
        sum["type"]=REFLESH_IP;
    }
    data["iplist"]=IPportlist;
    sum["data"]=data;
    string buf1 = sum.dump();
    Write(clie_fd, (char *)buf1.data(), buf1.length());
    Close(clie_fd);
    pthread_rwlock_unlock(&rw_lock);
}
void delayms(const int ms)
{
	struct timeval delay;
	
	delay.tv_sec = ms / 1000;
	delay.tv_usec = (ms % 1000 ) * 1000;

	select(0, NULL, NULL, NULL, &delay);
}