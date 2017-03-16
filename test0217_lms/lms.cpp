/*************************************************************************
    > File Name: lms.cpp
    > Author: sky
    > mail: 11211120@bjtu.edu.cn 
    > Created Time: Fri 17 Feb 2017 07:25:22 PM CST
    > target: ---------->   
    > how to use: ------>   
 ************************************************************************/

#include <iostream>
#include <string>
#include <unordered_map>

#include "solutions01.h"
#include "functions.h"

#include <event.h>
#include "myredis.h"
#include "idmp_pn.h"
#include "easyIdmp.h"


using namespace std;

Myredis myredis;
Trie trie;
Solution sov;
	int port;	//port是所有映射服务器共同的端口
	int sockfd_rms,sockfd_dms,sockfd_lms;
	int sockfd;	//该sockfd是 lms用于监听asr发来的消息，
				//当然，也可能是远程lms主动发起的发布消息。 
				//（远程lms被动发起的消息在sockfd_lms中接收）
string rmsIdstr;	//根映射服务器的地址
struct in_id rmsId;
struct sockaddr_id recurCli;	//lms向其它映射服务器递归发布

unordered_map<string, struct sockaddr_id> whoPublishMap;	//记录那个asr首先发起该aidstr的发布消息


int main()
{
	struct sockaddr_id lmser;	//bind到sockfd，处理asr,远程lms主动发起的消息
	struct event_base *base;

	string rmsIdstr;
	sov.readLmsConfig_fabu("lms.config",rmsIdstr,port,trie);
		if( idmp_pton(rmsIdstr.c_str(),&rmsId)<=0 )
		{
			cerr<<"lms.cpp: rms pton error..."<<endl;
			exit(1);
		}
	sockfd=Socket(AF_IDMP,SOCK_DGRAM,0);
		bzero(&lmser,sizeof(lmser));
		lmser.sid_family=AF_IDMP;
		lmser.sid_port=htons(port);
		lmser.sid_id=(struct in_id)ID_ANY_INIT;
	Bind(sockfd,(SA*)&lmser,sizeof(lmser));

	//初始化recurCli，用于lms向其他服务器递归发送消息.
	//只是为了方便设置其他服务器的 struct sockaddr而已，定义一个模板
		bzero(&recurCli,sizeof(recurCli));
		recurCli.sid_family=AF_IDMP;
		recurCli.sid_port=htons(port);

		//申请三个套接字用于递归查询
	sockfd_rms=Socket(AF_IDMP,SOCK_DGRAM,0);
	sockfd_dms=Socket(AF_IDMP,SOCK_DGRAM,0);
	sockfd_lms=Socket(AF_IDMP,SOCK_DGRAM,0);

	myredis.connect("127.0.0.1",0);

	base=event_base_new();

		//处理asr或者远端lms主动发起的信息
	struct event *evRcvfromAsr=event_new(base,sockfd,EV_READ|EV_PERSIST,onRcvfromAsr,NULL);
	struct event *evRcvfromLms=event_new(base,sockfd,EV_READ|EV_PERSIST,onRcvfromLms,NULL);
	struct event *evRcvfromDms=event_new(base,sockfd,EV_READ|EV_PERSIST,onRcvfromDms,NULL);
	struct event *evRcvfromRms=event_new(base,sockfd,EV_READ|EV_PERSIST,onRcvfromRms,NULL);

	event_add(evRcvfromAsr,NULL);
	event_add(evRcvfromLms,NULL);
	event_add(evRcvfromDms,NULL);
	event_add(evRcvfromRms,NULL);

	event_base_dispatch(base);


}
