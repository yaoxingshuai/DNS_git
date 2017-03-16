/*************************************************************************
    > File Name: solutions01.cpp
    > Author: sky
    > mail: 11211120@bjtu.edu.cn 
    > Created Time: Mon 06 Feb 2017 09:30:51 AM CST
    > target: ---------->   
    > how to use: ------>   
 ************************************************************************/

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <event.h>
#include <time.h>
#include "solutions01.h"
using namespace std;

	//读取 RMS的地址， 映射服务器约定工作的端口号(10011)， 该LMS自己管理的aid前缀
void Solution::readLmsConfig_fabu(const char *filepath, string &RMS, int &port, Trie &trie)
{
	ifstream cfgfile(filepath);
		if(!cfgfile)
		{
			cerr<<"open config file:"<<filepath<<" error..."<<endl;
			exit(1);
		}
	string buf;

	while( getline(cfgfile,buf))			
		//前面写asr.cpp中，这个地方是 cfgfile>>buf，遇空格分割，不太好
	{
		buf=strip(buf);
//cout<<"solutions: readfile buf: "<<buf<<endl;
			//注释
		if(buf=="" || buf[0]=='#')
			continue;
			//读取RMS的地址
		if(beginwith(buf,"RMS"))
		{
			int index=buf.find_first_of(":");
			RMS=buf.substr(index+1);
			RMS=strip(RMS);		//因为冒号后面可能还有空白
		}
			//读取端口
		else if(beginwith(buf,"PORT"))
		{
			int index=buf.find_first_of(":");
			string portstr=buf.substr(index+1);
			portstr=strip(portstr);
			port=atoi(portstr.c_str());
		}
			//读取lms管理的前缀
		else if(beginwith(buf,"PREFIX"))
		{
			//子循环，读取所有prefix
			while( getline(cfgfile,buf) )
			{
				buf=strip(buf);
				if( "END_PREFIX"==buf)
					break;
				if( buf=="" || buf[0]=='#')
					continue;
				string aid,preLen;
				int prefixLen;
				cutSpace(buf,aid,preLen);	//将 buf 切割为 aid和 前缀长  两个字段
				prefixLen=atoi(preLen.c_str());
				//插入到字典树,"~1"表示该aid前缀有自己管理
				trie.insert(aid,prefixLen,"~1");
			}
		}


	}
	return;
}


bool Solution::beginwith(const string &str, const string &pat)
{
	if(str.length()<=pat.length())
		return false;
	if( (str.find(pat)==0 ) && str[pat.length()]==':' )
		return true;
	else
		return false;
}

string Solution::strip(const string &str)
{
	int left =str.find_first_not_of(" \t\n");
	//如果为str全为space,left==npos,-1;
	if(left==string::npos)
		return "";

	int right =str.find_last_not_of(" \t\n");
	return str.substr(left,right-left+1);
}

bool Solution::cutSpace(const string &cstr, string &aid, string &rid)
{
	string str=strip(cstr);
	int spaceIndex=str.find_first_of(" \n\t");
	if(spaceIndex==string::npos)
	{
cout<<"warning: str has no space"<<endl;
		aid=str;
		rid="None";
		return false;
	}
	aid=str.substr(0,spaceIndex);
	rid=str.substr(spaceIndex+1);
	aid=strip(aid);
	rid=strip(rid);
	return true;
}

	//将字符串分为  三部分
bool Solution::cutSpace(const string &cstr, string &regist, string &aid, string &rid)
{
	string str=strip(cstr);
	//用户输入到fifo_fabu的信息应该是   1 aid rid 表示注册，  0 aid rid 表示注销
	string aid_rid;
	bool c1=cutSpace(str,regist,aid_rid);
	bool c2=cutSpace(aid_rid,aid,rid);

	if(c1&&c2)	
		return true;	//两次都切割正确，返回true
	return false;

}
bool Solution::cutSpace(const string &cstr, string &regist, string &aid, string &rid, string &status)
{
	string str=strip(cstr);
	string aid_rid_status;
	//cut into 4 pieces
	bool c1=cutSpace(str,regist,aid_rid_status);
	bool c2=cutSpace(aid_rid_status,aid,rid,status);

	if(c1&&c2)
		return true;
	return false;
}



