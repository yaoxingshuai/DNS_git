/*************************************************************************
    > File Name: myredis.h
    > Author: sky
    > mail: 11211120@bjtu.edu.cn 
    > Created Time: Mon 06 Feb 2017 09:47:23 AM CST
    > target: ---------->   
    > how to use: ------>   
 ************************************************************************/

#ifndef MYREDIS_H_
#define MYREDIS_H_

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "hiredis.h"

class Myredis
{
private:
	redisContext* con=NULL;

public:
	Myredis(){}
	~Myredis()
	{
		if(con!=NULL)
			redisFree(con);

		this->con=NULL;
	}
	void connect(const std::string &host, int port);
	void showAllKeys();
	void showKey(const std::string &aid);
	void showAllMap();
	void insertKeyVal(const std::string &key, const std::string &val);
	std::string getTypeOfKey(const std::string &key);
	std::string getValueOfKey(const std::string &key);

	int delKey(const std::string &key);



};

#endif
