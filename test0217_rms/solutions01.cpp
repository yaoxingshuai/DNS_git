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

	//映射服务器约定工作的端口号(10020)， 该RMS知道哪些aid前缀是哪个下级服务器管理
void Solution::readRmsConfig_fabu(const char *filepath, int &port, Trie &trie)
{
	ifstream cfgfile(filepath);
		if(!cfgfile)
		{
			cerr<<"solutions.cpp: open config file error..."<<endl;
			exit(1);
		}
	string buf;
	while( getline(cfgfile,buf) )
	{
		buf=strip(buf);
		if(buf=="" || buf[0]=='#')	//注释行
			continue;
		if(beginwith(buf,"PORT"))
		{
			int index=buf.find_first_of(":");
			string portstr=strip(buf.substr(index+1));
			port=atoi(portstr.c_str());
		}
		else if(beginwith(buf,"PREFIX"))
		{
			while( getline(cfgfile,buf) )
			{
				buf=strip(buf);
				if( buf=="END_PREFIX" )
					break;
				if( buf=="" || buf[0]=='#' )
					continue;
				string aidPre,preLen,msid;
				int prefixLen;
				cutSpace(buf,aidPre,preLen,msid);
				prefixLen=atoi(preLen.c_str());
				//插入到字典树
				trie.insert(aidPre,prefixLen,msid);
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



