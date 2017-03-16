/*************************************************************************
    > File Name: dictTree.h
    > Author: sky
    > mail: 11211120@bjtu.edu.cn 
    > Created Time: Wed 08 Feb 2017 04:43:48 PM CST
    > target: ---------->   
    > how to use: ------>   
 ************************************************************************/
#ifndef DICTTREE_H_
#define DICTTREE_H_

#include <iostream>
#include <string>
#include <vector>

#include "idmp_pn.h"

class TrieNode{
public:
	bool hasThisPrefix;			//字典树是否包含某个词（前缀）
	std::vector<TrieNode*> children;	//字典树以8位为单位记录前缀树
	std::string MSid;		//如果 hasThisPrefix==true, 则该 MSid 表示管理该前缀的映射服务器的id 

	//构造函数
	TrieNode(){		
		children=std::vector<TrieNode*>(256,NULL);
		hasThisPrefix=false;
		MSid="";
	}
	
};

class Trie{

private:
	TrieNode* root;
public:
		//构造函数
	Trie(){
		root=new TrieNode();
	}

	//将某个前缀和管理该前缀的map server id 插入到字典树
	void insert(const struct in_id &aidPrefix, int prefixLen, const struct in_id &msid);
	void insert(const struct in_id &aidPrefix, int prefixLen, const std::string &idstr);
	void insert(const std::string &aidPrefix, int prefixLen, const struct in_id &msid);
	void insert(const std::string &aidPrefix, int prefixLen, const std::string &idstr);

	//查找管理某个aid 对应的 服务器id
	std::string search(const struct in_id &aid);
	std::string search(const std::string &aidstr);
	
};


#endif
