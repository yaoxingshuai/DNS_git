/*************************************************************************
    > File Name: test_lms.cpp
    > Author: sky
    > mail: 11211120@bjtu.edu.cn 
    > Created Time: Fri 17 Feb 2017 07:03:06 PM CST
    > target: ---------->   写一个简单的lms来配合这个asr 
    > how to use: ------>   
 ************************************************************************/

#include <iostream>
#include <string>
#include "idmp_pn.h"
#include "easyIdmp.h"


using namespace std;

int main()
{
	char buffer[151];

	struct sockaddr_id ser;
		bzero(&ser,sizeof(ser));
		ser.sid_family=AF_IDMP;
		ser.sid_port=htons(10020);
		ser.sid_id=(struct in_id)ID_ANY_INIT;
	int sockfd=Socket(AF_IDMP,SOCK_DGRAM,0);
	Bind(sockfd,(SA*)&ser,sizeof(ser));
	while(1)
	{
		int n=recvfrom(sockfd,buffer,150,0,NULL,NULL);
		buffer[n]='\0';
		cout<<buffer<<endl;
	}
	return 0;
}
		
//测试已通过，asr对于从fifo收到的发布信息，
//会首先在本地redis库中做相应的注册注销处理，
//然后转发给lms
