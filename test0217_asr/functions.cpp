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

#include "functions.h"
#include "myredis.h"
#include "idmp_pn.h"
#include "solutions01.h"
using namespace std;

extern Myredis myredis;
extern int pipefd;
extern int sockfd;
extern sockaddr_id ser;	//lms的服务器地址
extern Solution sov;

//对fifo管道读取数据
void onReadFifo(int pip, short event, void *arg)
{
	FILE *fp=fdopen(pip,"r");
	char buffer[150];
	while( fgets(buffer,150,fp)!=NULL)
	{
		string aidstr,ridstr;
		bool regist;
		sov.cutSpace(buffer,regist,aidstr,ridstr);

			//如果是注册消息，asr收到也应该添加到redis数据库
		if(regist==true)
		{
			myredis.insertKeyVal(aidstr,ridstr);
		}
			//如果是注销消息，asr收到也应该在redis数据库中删除
		else
		{
			myredis.delKey(aidstr);
		}

//cout<<regist<<"___"<<aidstr<<"___"<<ridstr<<endl;
		
		//asr从管道收到注册/注销信息，转发给lms
		int n=sendto(sockfd,buffer,strlen(buffer),0,(SA*)&ser,sizeof(ser));
			if(n<=0)
			{
				cerr<<"functions.cpp: sendto error..."<<endl;
				exit(1);
			}
	}
cout<<"functions.cpp: asr read fifo one times ..."<<endl;
}

//从sockfd中读取数据，结果存放到redis，并且输出到fifo_out
void onReadFd(int fd, short ievent, void *arg)
{
	char buffer[128];
	int n;
	while( (n=recvfrom(fd,buffer,127,0,NULL,NULL)) > 0 )
	{
		buffer[n]='\0';
		cout<<"asr onReadFd: "<<buffer<<endl;
	}
}
