/*************************************************************************
    > File Name: solutions01.h
    > Author: sky
    > mail: 11211120@bjtu.edu.cn 
    > Created Time: Fri 17 Feb 2017 02:18:50 PM CST
    > target: ---------->   
    > how to use: ------>   
 ************************************************************************/

#ifndef SOLUTIONS01_H_
#define SOLUTIONS01_H_

#include <string>
#include "dictTree.h"

class Solution
{
public:
		//读取 RMS的地址， 映射服务器约定工作的端口号(10011)， 该LMS自己管理的aid前缀
	void readLmsConfig_fabu(const char *filepath, std::string &RMS, int &port, Trie &trie);
	

	//str是否以pat开始，且后面紧跟一个冒号
	bool beginwith(const std::string &str, const std::string &pat);

	//删除string两边的space
	std::string strip(const std::string &str);

	//将str通过空格分为aid,rid字符串
	bool cutSpace(const std::string &cstr, std::string &aid, std::string &rid);

	//将字符串分为  三部分，第一部分表示  注册还是注销
	//本函数在asr,lms的发布中有用，在rms和dms中有类似的cut为三部分的函数，但不完全一样。
	bool cutSpace(const std::string &cstr, std::string &regist, std::string &aid, std::string &rid);
	bool cutSpace(const std::string &cstr, std::string &regist, std::string &aid, std::string &rid, std::string &status);
};


#endif
