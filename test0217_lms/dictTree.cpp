/*************************************************************************
    > File Name: dictTree.cpp
    > Author: sky
    > mail: 11211120@bjtu.edu.cn 
    > Created Time: Wed 08 Feb 2017 05:03:01 PM CST
    > target: ---------->   
    > how to use: ------>   
 ************************************************************************/

#include <iostream>

#include "dictTree.h"
#include "idmp_pn.h"
using namespace std;




	//将某个前缀和管理该前缀的map server id 插入到字典树,其他重载的都调用这个函数
void Trie::insert(const struct in_id &aidPrefix, int prefixLen, const string &idstr)
{
	TrieNode *rootcpy=root;
	int size=prefixLen/8;	//前缀按照8位为一组
	for(int i=0;i<size;++i)
	{
		int index=(int)aidPrefix.s_id8[i];
//cout<<"dictTree.cpp: index="<<index<<endl;
		if(rootcpy->children[index]==NULL)
		{
			(rootcpy->children[index])=new TrieNode();
		}
		rootcpy=rootcpy->children[index];
	}

		//在字典树中插入节点，   记录管理该前缀的映射服务器地址MSid
	rootcpy->hasThisPrefix=true;
	rootcpy->MSid=idstr;
}
void Trie::insert(const struct in_id &aidPrefix, int prefixLen, const struct in_id &msid)
{
	char idstr[60];
	if( idmp_ntop(&msid,idstr,sizeof(struct sockaddr_id)) ==NULL)
	{
		cerr<<"dictTree.cpp: idmp_ntop = nullptr..."<<endl;
		exit(1);
	}
	insert(aidPrefix,prefixLen,idstr);
}
void Trie::insert(const string &aidPrefix, int prefixLen, const struct in_id &msid)
{
	struct in_id aid;
		if( idmp_pton(aidPrefix.c_str(),&aid)<=0 )
		{
			cerr<<"dictTree.cpp: pton error..."<<endl;
			exit(1);
		}
	char idstr[60];
		if( idmp_ntop(&msid,idstr,sizeof(struct sockaddr_id)) ==NULL)
		{
			cerr<<"dictTree.cpp: ntop = nullptr..."<<endl;
			exit(1);
		}
	insert(aid,prefixLen,idstr);
}
void Trie::insert(const string &aidPrefix, int prefixLen, const string &idstr)
{
	struct in_id aid;
		if( idmp_pton(aidPrefix.c_str(),&aid)<=0 )
		{
			cerr<<"dictTree.cpp: pton error..."<<endl;
			exit(1);
		}
	insert(aid,prefixLen,idstr);
}

	//查找管理某个aid 对应的 服务器id,返回string
string Trie::search(const struct in_id &aid)
{
	TrieNode *node=root;
	string msid;
	for(int i=0;i<16;++i)	//aid 128bit，共16个8位
	{
		int index=(int)aid.s_id8[i];

		if( node->children[index]==NULL )
			return msid;

		node=node->children[index];
		if( node->hasThisPrefix )
		{
			msid=node->MSid;
			//不用立即返回，尽量 最长前缀匹配
		}
	}
	return msid;
}
string Trie::search(const string &aidstr)
{
	struct in_id aid;
		if( idmp_pton(aidstr.c_str(),&aid)<=0 )
		{
			cerr<<"dictTree.cpp: pton error..."<<endl;
			exit(1);
		}
	return search(aid);
}
