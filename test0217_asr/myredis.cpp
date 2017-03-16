/*************************************************************************
    > File Name: myredis.cpp
    > Author: sky
    > mail: 11211120@bjtu.edu.cn 
    > Created Time: Mon 06 Feb 2017 09:48:12 AM CST
    > target: ---------->   
    > how to use: ------>   
 ************************************************************************/

#include <iostream>
#include <string>
#include <cstdlib>
#include <strings.h>

#include "hiredis.h"
#include "myredis.h"

using namespace std;

//连接redis数据库
void Myredis::connect(const string &host, int port)
{
	if(port<=0)		//如果不指定端口，使用默认端口6379
		port=6379;
	con=redisConnect(host.c_str(),port);
	if(con==NULL || con->err)
	{
		cerr<<"redis connect error..."<<endl;
		exit(1);
	}
cout<<"redis connect ok"<<endl;
}

//keys *
void Myredis::showAllKeys()
{
	redisReply* rep=(redisReply*)redisCommand(con,"keys *");
	if(rep==NULL)
	{
		cerr<<"command keys * error..."<<endl;
		exit(1);
	}
	struct redisReply **ret=rep->element;
	for(int i=0;i<rep->elements;++i)
	{
		cout<<(*ret)->str<<endl;
		++ret;
	}
	freeReplyObject(rep);
}

//输出键值对
void Myredis::showKey(const string &aid)
{
	//if key val is not string
	//如果key对应的不是字符串，应该会rep->type==REDIS_REPLY_ERR
	redisReply* rep=(redisReply*)redisCommand(con,"get %s",aid.c_str());

	//cout<<rep->type<<"\t"<<rep->str<<"\t"<<aid<<endl;
	//如果redisCommand命令执行错误，返回NULL，设置rep->str的error信息
	if(rep==NULL || rep->type==REDIS_ERR)
	{
		cerr<<"command get key error..."<<rep->str<<endl;
		exit(1);
	}
	else if(rep->type==REDIS_REPLY_NIL)
	{
		cout<<"aid not exits:"<<aid<<endl;
		return;
	}
	else if(rep->type==REDIS_REPLY_ERROR)
	{
		cout<<"aid:"<<aid<<"\t value is not string"<<endl;
		return;
	}
	else if(rep->type==REDIS_REPLY_STRING)
	{
		cout<<aid<<"\t"<<rep->str<<endl;
		return;
	}
	freeReplyObject(rep);
}

//列出所有的键值对，对每一个key依次调用showkey()
void Myredis::showAllMap()
{
	redisReply* rep=(redisReply*)redisCommand(con,"keys *");
	if(rep==NULL)
	{
		cerr<<"showAllMap error..."<<endl;
		exit(1);
	}
	struct redisReply **ret=rep->element;
	for(int i=0;i<rep->elements;++i)
	{
		showKey(string((*ret)->str) );
		++ret;
	}
	freeReplyObject(rep);
}

//插入键值对
void Myredis::insertKeyVal(const string& key, const string& val)
{
	redisReply* rep=(redisReply*)redisCommand(con,"set %s %s",key.c_str(),val.c_str());
	if(rep==NULL)
	{
		cerr<<"set key val error"<<endl;
		exit(1);
	}
	if( rep->type!=REDIS_REPLY_STATUS || strcasecmp(rep->str,"ok")!=0 )
	{
		cerr<<"set key val error"<<endl;
	}
cout<<"set ok!  aid = "<<key<<"\t rid = "<<val<<endl;
	freeReplyObject(rep);
}

//获取key的存储类型
string Myredis::getTypeOfKey(const string &key)
{
	redisReply *rep=(redisReply*)redisCommand(con,"type %s",key.c_str());
	if(rep==NULL)
	{
		cerr<<"type error..."<<endl;
		exit(1);
	}
	string type(rep->str);
	freeReplyObject(rep);
	return type;
}

//获取key的对应的val，如果不是string，返回None
string Myredis::getValueOfKey(const string &key)
{
	if( getTypeOfKey(key)!="string")
		return "None";
	redisReply *rep=(redisReply*)redisCommand(con,"get %s",key.c_str());
	if(rep==NULL)
	{
		cerr<<"get value error..."<<endl;
		exit(1);
	}
	if(rep->type==REDIS_REPLY_NIL)
		return "None";
	return rep->str;
}


int Myredis::delKey(const std::string &key)
{
	redisReply *rep=(redisReply*)redisCommand(con,"del %s",key.c_str());

	if(rep==NULL || rep->type==REDIS_ERR || rep->type==REDIS_REPLY_ERROR)
	{
		cerr<<"del key error..."<<endl;
	}
	int lines=-1;	//受影响的行数
	if(rep->type==REDIS_REPLY_INTEGER)
	{
		lines=rep->integer;
	}

	freeReplyObject(rep);
	return lines;
}
	
