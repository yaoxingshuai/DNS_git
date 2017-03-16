/*************************************************************************
    > File Name: asr.cpp
    > Author: sky
    > mail: 11211120@bjtu.edu.cn 
    > Created Time: Fri 17 Feb 2017 02:05:15 PM CST
    > target: ---------->   
    > how to use: ------>   
 ************************************************************************/

#include <iostream>
#include <string>
#include <cstdlib>

#include <event.h>
#include <sys/types.h>	//mkfifo需要这两个头文件
#include <sys/stat.h>
#include <fcntl.h>		//o_rdwr之类的在该头文件中定义

#include "easyIdmp.h"
#include "idmp_pn.h"
#include "solutions01.h"
#include "myredis.h"
#include "functions.h"

using namespace std;

Myredis myredis;
int pipefd;

int sockfd;	//用于asr向lms发布注册或注销映射关系
struct sockaddr_id ser;
Solution sov;

int main()
{
	string LMSidstr;
	int port;

	//读取配置文件，获取lms的id和port
	sov.readAsrConfig_fabu("asr.config",LMSidstr,port);

	sockfd=Socket(AF_IDMP,SOCK_DGRAM,0);
		bzero(&ser,sizeof(ser));
		ser.sid_family=AF_IDMP;
		ser.sid_port=htons(port);
		if( idmp_pton(LMSidstr.c_str(),&ser.sid_id)<=0 )
		{
			cerr<<"asr.cpp: pton error..."<<endl;
			exit(1);
		}

	myredis.connect("127.0.0.1",0);
			//使用默认端口连接redis

	string fifoName="/tmp/asr_fabu";
			//创建管道，用于读取发布的消息
	if( access(fifoName.c_str(),F_OK) == -1 )
	{
		if(mkfifo(fifoName.c_str(),0777)!=0)
		{
			cerr<<"cannot create fifo:"<<fifoName<<endl;
			exit(1);
		}
	}

	int justForOpen_fd=open(fifoName.c_str(),O_RDWR);
			//单纯的占据管道，防止cat之类的忙等待
	pipefd=open(fifoName.c_str(),O_RDONLY);

			if( pipefd<0 || justForOpen_fd<0 )
			{
				cerr<<"asr: open fifo error..."<<endl;
				unlink(fifoName.c_str());
				exit(1);
			}
	evutil_make_socket_nonblocking(pipefd);
	evutil_make_socket_nonblocking(sockfd);
			//设置fd不阻塞，否则当fifo没有数据是，read或fgets会阻塞
			//这样设置以后，read直接返回0，fgets返回NULL
	struct event_base *base=event_base_new();

	//fifo有待发布的消息传进来时，通过onReadFifo函数处理之
	struct event *evReadFifo=event_new(base,pipefd,EV_READ|EV_PERSIST,onReadFifo,NULL);

	struct event *evReadFd=event_new(base,sockfd,EV_READ|EV_PERSIST,onReadFd,NULL);
	

	event_add(evReadFifo,NULL);
	event_add(evReadFd,NULL);
	event_base_dispatch(base);

	return 0;
}
