/*************************************************************************
    > File Name: functions.cpp
    > Author: sky
    > mail: 11211120@bjtu.edu.cn 
    > Created Time: Mon 06 Feb 2017 03:55:14 PM CST
    > target: ---------->   
    > how to use: ------>   
 ************************************************************************/

#include <iostream>
#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <unordered_map>

#include "functions.h"
#include "myredis.h"
#include "idmp_pn.h"
#include "solutions01.h"
using namespace std;


	extern int sockfd_rms,sockfd_dms,sockfd_lms;
	extern int port;
	extern int sockfd;
extern struct in_id rmsId;

extern Trie trie;
extern Myredis myredis;
extern Solution sov;

extern unordered_map<string, struct sockaddr_id> whoPublishMap;	
extern struct sockaddr_id recurCli;	//lms向其他映射服务器递归查询

//当asr传来<regist,aid,rid>发布时，处理        
//impor: 也可能是远端lms主动发起的aid查询，但是处理方法都一样
//如果是该lms自己管理，直接发布成功,
//如果不是该lms自己管理，递归
void onRcvfromAsr(int fd, short ievent, void *arg)
{
	struct sockaddr_id cli;
	socklen_t clilen=sizeof(cli);

	char buffer[128];
	int n=recvfrom(fd,buffer,127,0,(SA*)&cli,&clilen);
			if(n<0){
				cerr<<"functions.cpp: recvfrom error..."<<endl;
				exit(1);
			}
	buffer[n]='\0';
cout<<"functions.cpp: recvfrom "<<n<<" bytes from asr:"<<buffer<<endl;
//	strncat(buffer," hello,world",13);

	string regist;
	string aidstr,ridstr;
	sov.cutSpace(buffer,regist,aidstr,ridstr);

	struct in_id aid;
		if( idmp_pton(aidstr.c_str(),&aid)<=0 )
		{
			cerr<<"warning: functions.cpp: requestid pton error..."<<endl;
			return;	
			//感觉return好些，不能因为乱七八糟的输入导致程序退出
//			exit(1);
		}
	
	//看LMS归谁管辖
	string manageMSid=trie.search(aid);	//获取管辖该aid的映射服务器
//该lms自己管辖的
	if(manageMSid=="~1")
	{
cout<<"自己管辖"<<endl;
		string result;

		//注册或注销的处理
		if(regist=="1" || regist=="add")
		{
			myredis.insertKeyVal(aidstr,ridstr);
			result="add "+aidstr+'\t'+ridstr+" ok";
		}
		else
		{
			myredis.delKey(aidstr);
			result="del "+aidstr+'\t'+ridstr+" ok";
		}

		//返回给cli，直接利用回调函数的fd即可
			if( sendto(fd,result.c_str(),result.length(),0,(SA*)&cli,sizeof(cli)) <=0 )
			{
				cerr<<"functions.cpp: sendto error..."<<endl;
				exit(1);
			}
		return;			//回调函数完成
	}
//远端映射缓存表没有，向rms发起查询
	else if(manageMSid=="")
	{
cout<<"向rms递归查询"<<endl;
		//recurCli.sid_id=rmsId;	recurCli会有很多回调函数共同使用，所以不要改变他
		struct sockaddr_id cliRms=recurCli;
		cliRms.sid_id=rmsId;
		//记录是哪个asr发起的查询
		whoPublishMap[aidstr]=cli;
		//递归向rms发起查询
			if( sendto(sockfd_rms,buffer,strlen(buffer),0,(SA*)&cliRms,sizeof(cliRms)) <= 0)
			{
				cerr<<"functions.cpp rms recur sendto error..."<<endl;
				exit(1);
			}
			//向rms递归查询，rms返回的结果交给onRcvfromRms()处理即可
		return;
	}
//记录有其他那个lms管理该aid, 那就向该lms递归发起查询
	else
	{
cout<<"向远端lms递归查询"<<endl;
		struct sockaddr_id cliLms=recurCli;
		if( idmp_pton(manageMSid.c_str(),&cliLms.sid_id) <=0 )
		{
			cerr<<"functions.cpp: manageid pton error..."<<endl;
			exit(1);
		}
		//记录是哪个asr发起的查询
		whoPublishMap[aidstr]=cli;
		//递归向lms发起查询
			if( sendto(sockfd_lms,buffer,strlen(buffer),0,(SA*)&cliLms,sizeof(cliLms)) <= 0)
			{
				cerr<<"functions.cpp: lms recur sendto error..."<<endl;
				exit(1);
			}
		//向lms递归查询，远端lms返回的结果交给onRcvfromLms()即可
		return;
	}
cout<<"---onRcvfromAsr callback"<<endl;
}

//远端lms告诉该lms <regist aid \t rid> 的映射关系
//该lms收到消息后，查询 whoPublishMap 找到那个asr想查询这个aid
//然后返回这个信息给asr即可
void onRcvfromLms(int sock, short ievent, void *arg)
{
	char buffer[128];	int n;
		if( (n=recvfrom(sock,buffer,127,0,NULL,NULL))<0 )
		{
			cerr<<"functions.cpp: recvfrom dms error..."<<endl;
			exit(1);
		}
	buffer[n]='\0';
	
	string regist,aidstr,ridstr,status;
	if( sov.cutSpace(buffer,regist,aidstr,ridstr,status)==false )
	{
		cerr<<"functions.cpp: cutspace <aid,rid> error..."<<endl;
		exit(1);
	}
	//如果远端的映射服务器返回<regi,aid,rid,None>, 则交给 recurNone()处理
	//recurNone()向asr返回 <aid,None>
	if( ridstr=="" || ridstr=="None" )
	{
		recurNone(aidstr);
		return;	//交给recurNone()处理，直接return;
	}
	//
	//查询 whoPublishMap 看是那个asr想查询这个aid
	if( whoPublishMap.find(aidstr)== whoPublishMap.end() )
	{
		cerr<<"应该有asr首先发起这个查询的"<<endl;
		return;
	}
	struct sockaddr_id asr=whoPublishMap[aidstr];
	//返回asr结果
	//sockfd是专门面向asr的连接
	if( sendto(sockfd,buffer,strlen(buffer),0,(SA*)&asr,sizeof(asr)) <= 0)
	{
		cerr<<"functions.cpp: lms sendto asr error..."<<endl;
		exit(1);
	}
	//asr现在不关心aid了， whoPublishMap清除该key
	whoPublishMap.erase(aidstr);
	return;
}

//dms告诉该lms   <regist aid \t rid \t lms> 那个lms管理该aid
//该回调函数处理dms发来的信息，提取出aid和lmsid，递归向lms发起查询
//参照onRcvfromRms函数写即可
//
//
//我暂时写两层，onRcvfromDms不管，以后再更改
void onRcvfromDms(int sock, short ievent, void *arg)
{
	char buffer[128];	int n;
		if( (n=recvfrom(sock,buffer,127,0,NULL,NULL))<0 )
		{
			cerr<<"functions.cpp: recvfrom dms error..."<<endl;
			exit(1);
		}
	buffer[n]='\0';
	string result(buffer);
	result=sov.strip(result);


	string requestAid,manageLms;
	if( sov.cutSpace(result,requestAid,manageLms)==false )
	{
		cerr<<"functions.cpp: cutspace <aid,LmsId> error..."<<endl;
		exit(1);
	}
	//如果远端的映射服务器返回<aid,None>, 则交给 recurNone()处理
	//recurNone()向asr返回 <aid,None>
	if( manageLms=="" || manageLms=="None" )
	{
		recurNone(requestAid);
		return;
	}

	//这里dms返回的<aid, lms>映射关系
	//如果完整的返回 映射应答包，里面可能包含下级lms管理的aid前缀，这个映射关系需要加入到trie中

	//递归向LMS发起查询
	struct sockaddr_id cliLms=recurCli;
	if( idmp_pton(manageLms.c_str(),&cliLms.sid_id)<=0 )
	{
		cerr<<"functions.cpp: pton lms error..."<<endl;
		exit(1);
	}
	if( sendto(sockfd_lms,requestAid.c_str(),requestAid.length(),0,(SA*)&cliLms,sizeof(cliLms)) <= 0)
	{
		cerr<<"functions.cpp: lms recur sendto error..."<<endl;
		exit(1);
	}
	//远端lms返回的数据交给onRcvfromLms处理即可
	return;
}

//rms告诉该lms  <regist aid \t rid \t dms>  哪个dms管理该aid
//              <bool string string string>
//该回调函数处理rms发来的信息，提取出aid和dmsid，递归向dms发起查询
void onRcvfromRms(int sock, short ievent, void *arg)
{
	char buffer[128];	int n;
		if( (n=recvfrom(sock,buffer,127,0,NULL,NULL))<0 )
		{
			cerr<<"functions.cpp: recvfrom rms error..."<<endl;
			exit(1);
		}
	buffer[n]='\0';

	string regist,aidstr,ridstr,dmstr;   

	sov.cutSpace(buffer,regist,aidstr,ridstr,dmstr);


	//如果远端的映射服务器返回dmstr=="None",表示没有服务器管理该aid
	//则交给 recurNone()处理
	//recurNone()向asr返回 "no ms manage this aid"
	if( dmstr=="" || dmstr=="None" )
	{
		recurNone(aidstr);
		return;
	}

	//递归向DMS发起查询
	struct sockaddr_id cliDms=recurCli;
	if( idmp_pton(dmstr.c_str(),&cliDms.sid_id) <=0 )
	{
		cerr<<"functions.cpp: pton dms error..."<<endl;
		exit(1);
	}
	/*
	if( sendto(sockfd_dms,requestAid.c_str(),requestAid.length(),0,(SA*)&cliDms,sizeof(cliDms)) <= 0)
	{
		cerr<<"functions.cpp: dms recur sendto error..."<<endl;
		exit(1);
	}
	*/
	//可以改为两层结构，去掉DMS层
	//即RMS返回的消息通过直接向远端LMS查询
	//
		//填充发布的消息
	string fabu=regist+' '+aidstr+'\t'+ridstr;
	u_int cliLen=sizeof(cliDms);
	if( sendto(sockfd_lms,fabu.c_str(),fabu.length(),0,(SA*)&cliDms,cliLen) <= 0)
	{
		cerr<<"functions.cpp: lms recur sendto error..."<<endl;
		exit(1);
	}
	//因为实验室机器不够，所以暂时跳过dms层
		


	//dms返回的信息交给onRcvfromDms处理即可
	return ;

}

//如果rms,dms,lms向lms回复 <aid,"None"> 表示该aid对应的映射关系不存在时，
//lms也应该向发起该aid查询的asr返回 <aid,"None">
void recurNone(const std::string &aidstr)
{
	if( whoPublishMap.find(aidstr)== whoPublishMap.end() )
	{
		//没有asr关心这个aidstr
		return;
	}
	struct sockaddr_id asr=whoPublishMap[aidstr];
	whoPublishMap.erase(aidstr);
	string result=aidstr+'\t'+"None";	
	//impor:这里其实最好返回<regi,aid,rid,none>
			//在发布中，表示没有服务器管理该aid

	if(  sendto(sockfd,result.c_str(),result.length(),0,(SA*)&asr,sizeof(asr)) <= 0 )
	{
		cerr<<"functions.cpp: sendto asr error..."<<endl;
		exit(1);
	}
	return;
}
