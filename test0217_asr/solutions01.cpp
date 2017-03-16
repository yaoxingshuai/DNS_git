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

void Solution::readAsrConfig_fabu(const char *filepath, string &LMS, int &port)
{
	ifstream cfgfile(filepath);	
		//打开文件关联到ifstream上
	if(!cfgfile)
	{
		cout<<"open config file: "<<filepath<<" error!"<<endl;
		exit(0);
	}
	string buf;
	while( getline(cfgfile,buf) )
		//读取文件内容到buf
	{
		buf=strip(buf);
			//如果buf是注释
		if(""==buf || '#'==buf[0])
			continue;
		if( beginwith(buf,"LMS") )
		{
			int index=buf.find_first_of(':');
			LMS=buf.substr(index+1);
			LMS=strip(LMS);
		}
		else if( beginwith(buf,"PORT") )
		{
			int index=buf.find_first_of(':');
			string portstr=buf.substr(index+1);
			portstr=strip(portstr);
			port=atoi(portstr.c_str());
		}
	}
	cout<<"read ok"<<endl;
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

	//将字符串分为  三部分，第一部分表示  注册还是注销
bool Solution::cutSpace(const std::string &cstr, bool &regist, std::string &aid, std::string &rid)
{
	string str=strip(cstr);
	//用户输入到fifo_fabu的信息应该是   1 aid rid 表示注册，  0 aid rid 表示注销
	string zhuce_zhuxiao;
	string aid_rid;
	bool c1=cutSpace(str,zhuce_zhuxiao,aid_rid);
	bool c2=cutSpace(aid_rid,aid,rid);

	if("1"==zhuce_zhuxiao)
		regist=true;
	else
		regist=false;	//表示注销

	if(c1&&c2)	
		return true;	//两次都切割正确，返回true
	return false;

}




