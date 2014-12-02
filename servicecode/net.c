#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include "config.h"
#include "header.h"
//规定传输格式:程序包头(4)+  标志终端类型(1) + 操作类型(1)  
//  数据大小(2)+ 数据

int net_init(struct sockaddr_in *server_addr)
{
	int sock_fd;
	if((sock_fd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		printf("%s %d: Create Socket error\n",__func__,__LINE__);
		return -1;
	}
	bzero(server_addr,sizeof(struct sockaddr_in));
	server_addr->sin_family = AF_INET;
	server_addr->sin_addr.s_addr = inet_addr(SERVERS_IP);
	server_addr->sin_port = htons(PORT_NUMBER);

	if(bind(sock_fd,(struct sockaddr *)server_addr,sizeof(struct sockaddr))==-1)
	{
		printf("%s %d:bind error \n",__func__,__LINE__);
		return -1;
	}		
	if(listen(sock_fd,MAX_CLINT_NUMBER)==-1)
	{
		printf("%s %d:listen error \n",__func__,__LINE__);
		return -1;
	}

	return sock_fd;
}
int check_data_head(char buf[])
{
	int pack_head = buf[0] + (buf[1]<<8) + (buf[2]<<16) + (buf[3]<<24);
	return (PACKAGE_HEAD == pack_head)?0:-1;
}
enum client_kind get_client_kind(char n1,char n2)
{
	int i = n1+(n2<<8);
	if(ANDROID_CLIENT == i)
	{
		printf("Android client\n");
		return ANDROID;
	}
	else if(PC_CLIENT == i)
	{
		printf("pc client\n");
		return PC;
	}
	else return -1;
}	

int send_ok_toclient(int fd,char buf[],int size)
{
	char pakage[1024] = {0x26,0x04,0x19,0x19,0x34,0x12,SEND_CMD_TO_CLINET,2,0x66,0x66};
	int i ;
	size += 10;
	for(i = 10;i < size; i++)
	{
		pakage[i] = buf[i-10]; 
	}
	if(size == write(fd,pakage,size))
	{
		printf("send message ok\n");
		return 0;
	}
	else printf("write back to client error\n");
	return -1;
}
int send_wrong_toclient(int fd)
{
	char pakage[] = {0x26,0x04,0x19,0x19,0x34,0x12,SEND_CMD_TO_CLINET,2,0x44,0x44};
	if(10 == write(fd,pakage,10))return 0;
	else return -1;

}
void * service_handle_thread(void *arg)
{
	struct thread_param *param = (struct thread_param *)arg;
	int sin_size;
	int nbytes;
	char buffer[1025];
	int i;	
	struct tcp_param p;

	if(pthread_detach(pthread_self()))printf("pthread detach error!\n",__func__,__LINE__);
	
	p.ip = inet_ntoa(param->sockaddr->sin_addr);
	p.fd = param->fd;
	printf("%s %d:server get connetction from %s\n",__func__,__LINE__,p.ip);
	nbytes = read(param->fd,buffer,1024);
	if(-1 == nbytes)
	{
		printf("%s %d :read error nbytes = %d \n",__func__,__LINE__,nbytes);
		pthread_exit((void*)1);
	}
	for(i = 0;i < nbytes; i++)printf("i = %d,%c  0x%x\n",i,buffer[i],buffer[i]);

	if(check_data_head(&buffer[0]))
	{	
		printf("__func__,__LINE__ pakage head wrong\n");
		pthread_exit((void*)1);
		
	}
	p.kind = get_client_kind(buffer[4],buffer[5]);
	if(-1 == p.kind)
	{
		printf("__func__,__LINE__ client kind wrong\n");
		pthread_exit((void*)1);
	}

	if(!handle_cmd_data(&buffer[6],&p))
	{
		printf("cammand handle success\n");
	//	send_ok_toclient(param->fd);
	}else{
		send_wrong_toclient(param->fd);
	}

	close(param->fd);	
	pthread_exit((void*)1);

}
void creat_thread(struct thread_param* param)
{
	pthread_t t;
	pthread_create(&t,NULL,service_handle_thread,param);
}
int start_net(int socket_fd ,struct sockaddr_in* client_addr)
{
	int fd;
	int size = sizeof(struct sockaddr_in);	
	struct thread_param param;
	param.sockaddr = client_addr;
	while(1)
	{
		if((fd = accept(socket_fd,(struct sockaddr *)client_addr,&size))==-1)
		{
			printf("%s %d accept error \n",__func__,__LINE__);
			exit(1);
		}
		param.fd = fd;
		creat_thread(&param);
	}
	return 0;
}

