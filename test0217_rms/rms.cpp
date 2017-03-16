/*************************************************************************
    > File Name: rms.cpp
    > Author: sky
    > mail: 11211120@bjtu.edu.cn 
    > Created Time: Mon 13 Feb 2017 08:13:27 PM CST
    > target: ---------->   
    > how to use: ------>   
 ************************************************************************/

#include <iostream>
#include <string>

#include "dictTree.h"
#include "solutions01.h"
#include "easyIdmp.h"
#include "idmp_pn.h"

using namespace std;

//rms和dms的app都不需要redis操作
int main()
{
	Trie trie;
	Solution sov;
	int port,sockfd;

	sov.readRmsConfig_fabu("rms.config",port,trie);

	//rms服务器
	struct sockaddr_id ser,cli;
		bzero(&ser,sizeof(ser));
		ser.sid_family=AF_IDMP;
		ser.sid_id=(struct in_id)ID_ANY_INIT;
		ser.sid_port=htons(port);
	sockfd=Socket(AF_IDMP,SOCK_DGRAM,0);
	Bind(sockfd,(SA*)&ser,sizeof(ser));

	int n;
	char buffer[128];
	struct in_id aid;
cout<<"begin while(1)..."<<endl;
	while(1)
	{
		socklen_t cliLen=sizeof(cli);
		n=recvfrom(sockfd,buffer,127,0,(SA*)&cli,&cliLen);
		if(n<=0)
		{
			cerr<<"recvfrom error..."<<endl;
			exit(1);
		}
		buffer[n]='\0';

		string regist,aidstr,ridstr;
		sov.cutSpace(buffer,regist,aidstr,ridstr);
		if( idmp_pton(aidstr.c_str(),&aid)<=0 )
		{
			cerr<<"pton error..."<<buffer<<endl;
			exit(1);
		}
		string msid=trie.search(aid);
		//如果没找到，rms都没找到说明没有这个aid映射关系
		if(msid=="")
			msid="None";

		string result=regist+' '+aidstr+'\t'+ridstr+' '+msid;
				//rms返回 regist,aid,rid,msid。   
				//如果msid==none，lms收到后会直接交给recurNone()处理
cout<<__FILE__<<__LINE__<<result<<endl;
		n=sendto(sockfd,result.c_str(),result.length(),0,(SA*)&cli,sizeof(cli));
			if(n<result.length())
			{
				cerr<<"sendto error..."<<endl;
				exit(1);
			}
	}


}
