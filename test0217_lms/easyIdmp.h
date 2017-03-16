#ifndef EASYIDMP_H_
#define EASYIDMP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/idmp.h>
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <netpacket/packet.h>
#include <net/if.h>
#include <netinet/ucmp.h>
#include <arpa/inet.h>

int startup_v4_tcp(u_short *pport);
int startup_idmp_udp(u_short *pport);
int Socket(int family,int type,int protocol) ;
void Bind(int sockfd,const struct sockaddr* sa,socklen_t salen);
void Listen(int fd,int backlog);
int Accept(int fd,struct sockaddr* sa,socklen_t *salenptr);
void Connect(int fd,const struct sockaddr* sa,socklen_t salen);

#define SA struct sockaddr
#define MY_BUFLEN 255

//用于socket，bind，listen更方便，有v4和idmp两个版本
int startup_v4_tcp(u_short *pport)
{
	int sockfd=0;
	struct sockaddr_in ser;
	sockfd=Socket(AF_INET,SOCK_STREAM,0);
	
	memset(&ser,0,sizeof(ser));

	ser.sin_family=AF_INET;
	ser.sin_port=htons(*pport);
	ser.sin_addr.s_addr=htonl(INADDR_ANY);

	Bind(sockfd,(struct sockaddr*)&ser,sizeof(ser));

	//如果指定的端口为0，则随机分配一个端口
	if(*pport==0)
	{
		unsigned int serlen=sizeof(ser);
		if( getsockname(sockfd,(struct sockaddr*)&ser,&serlen)==-1 )
		{
			perror("getsockname error");
			exit(1);
		}
		*pport=ntohs(ser.sin_port);
	}
	Listen(sockfd,5);
	return sockfd;
}
//idmp的udp版本
int startup_idmp_udp(u_short *pport)
{
	struct sockaddr_id ser;
	memset(&ser,0,sizeof(ser));
	ser.sid_family=AF_IDMP;
	ser.sid_id=(struct in_id)ID_ANY_INIT;
	ser.sid_port=htons(*pport);

	int sockfd=Socket(AF_IDMP,SOCK_DGRAM,0);
	Bind(sockfd,(struct sockaddr*)&ser,sizeof(ser));

	if(*pport==0)
	{
		unsigned int serlen=sizeof(ser);
		if( getsockname(sockfd,(struct sockaddr*)&ser,&serlen)==-1 )
		{
			perror("getsockname error");
			exit(1);
		}
		*pport=ntohs(ser.sid_port);
	}
	return sockfd;
}
		


int Socket(int family,int type,int protocol) 
{
	int n;
	if( (n=socket(family,type,protocol))<0 ){
		fprintf(stderr,"socket() create error:%s\n",strerror(errno));
		exit(1);
	}
	return n;
}

void Bind(int sockfd,const struct sockaddr* sa,socklen_t salen)
{
	if( bind(sockfd,sa,salen)<0 ){
		fprintf(stderr,"bind() error:%s\n",strerror(errno));
		exit(1);
	}
}

void Listen(int fd,int backlog)
{
	char *ptr;

	if( (ptr=getenv("LISTENQ"))!=NULL )
		backlog=atoi(ptr);
	if( listen(fd,backlog)<0 ){
		fprintf(stderr,"listen() error:%s\n",strerror(errno));
		exit(1);
	}
}

int Accept(int fd,struct sockaddr* sa,socklen_t *salenptr)
{
	int n;
	if( (n=accept(fd,sa,salenptr))<0 ){
		fprintf(stderr,"accept() error:%s\n",strerror(errno));
		exit(1);
	}
	return n;
}

void Connect(int fd,const struct sockaddr* sa,socklen_t salen)
{
	if( connect(fd,sa,salen)<0 ){
		fprintf(stderr,"connect() error:%s\n",strerror(errno));
		exit(1);
	}
}




#endif
