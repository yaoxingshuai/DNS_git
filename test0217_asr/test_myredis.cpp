/*************************************************************************
    > File Name: test_myredis.cpp
    > Author: sky
    > mail: 11211120@bjtu.edu.cn 
    > Created Time: Fri 17 Feb 2017 06:46:09 PM CST
    > target: ---------->   
    > how to use: ------>   
 ************************************************************************/

#include <iostream>

#include "myredis.h"
using namespace std;
int main()
{
	Myredis myredis;

	myredis.connect("127.0.0.1",0);

	myredis.showAllMap();

	cout<<"---------------------"<<endl;
	cout<<myredis.delKey("k1");
	cout<<myredis.delKey("k2");
	cout<<myredis.delKey("k3");

	cout<<"---------------------"<<endl;
	myredis.showAllMap();

	return 0;
}


