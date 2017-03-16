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

class Solution
{
public:
	void readAsrConfig_fabu(const char *filepath, std::string &LMSidstr, int &port);

	//str是否以pat开始，且后面紧跟一个冒号
	bool beginwith(const std::string &str, const std::string &pat);

	//删除string两边的space
	std::string strip(const std::string &str);

	//将str通过空格分为aid,rid字符串
	bool cutSpace(const std::string &cstr, std::string &aid, std::string &rid);

	//将字符串分为  三部分，第一部分表示  注册还是注销
	bool cutSpace(const std::string &cstr, bool &regist, std::string &aid, std::string &rid);
};


#endif
