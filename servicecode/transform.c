#include "config.h"
#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#define UDP_PORT_NUMBER 13334
#define UDP_MESSAGEBUF_COUNT 100
#define MAXDATASIZE 1024
struct udpdata_handle_param
{
	char ip[20];
	int port;
	struct sockaddr_in *p_sockaddr;
};
 int start_udp_transform(void)	
 {	
	 int mysock,len;  
	int addr_len;	
	 struct sockaddr_in addr;  
	 struct sockaddr_in to_addr;  
	 char msg[256];  
	struct udpdata_handle_param p;	 
	   
	 if (( mysock= socket(AF_INET, SOCK_DGRAM, 0) )<0)	
	 {	
		 perror("error");  
		 exit(1);  
	 }	
	 else  
	 {	
		 printf("socket created .\n");	
		 printf("socked id: %d \n",mysock);  
	 }	
	  
	 addr_len=sizeof(struct sockaddr_in);  
	 bzero(&addr,sizeof(addr));  
	 addr.sin_family=AF_INET;  
	 addr.sin_port=htons(UDP_PORT_NUMBER);  
	 addr.sin_addr.s_addr=htonl(INADDR_ANY);  
	  
	  
	 if(bind(mysock,&addr,sizeof(addr))<0)	
	 {	
		 perror("connect");  
		 exit(1);  
	 }	
	 else  
	 {	
		 printf("bind ok.\n");	
		 printf("local port :%d \n",UDP_PORT_NUMBER);  
	 }	
	 
	 while (1)	
	 {	
		 bzero(msg,sizeof(msg));  
		 len= recvfrom (mysock,msg,sizeof(msg),0,&addr,&addr_len);/*?????*/  
		 printf("message from : %s port=%d\n",inet_ntoa(addr.sin_addr),htons(addr.sin_port));
	
		 p.p_sockaddr = &addr;
		 udpdata_handle(msg,&p,mysock); 
		 
		 bzero(&to_addr,sizeof(to_addr)); 
		 to_addr = addr; 
		 to_addr.sin_port=htons(p.port);  
		inet_aton(p.ip,&to_addr.sin_addr);
		len =  sendto(mysock,msg,len,0,&to_addr,addr_len); /*????????????*/	
		printf("len = %d\n",len);
	 }	  
 }	



 int udpdata_handle(char *buffer,struct udpdata_handle_param *p ,int sockfd)
{
	char lenth[2];
	char cmd1[100];
	char cmd2[100];
	
	char cmd3[100];
	char cmd4[100];
	char user_name[20];
	char *source_ip = inet_ntoa(p->p_sockaddr->sin_addr);
	int num = 0;	
	enum client_kind kind;
	struct sockaddr_in to;
	
	if(check_data_head(&buffer[0]))
	{	
		printf("__func__,__LINE__ pakage head wrong\n");
		return -1;		
	}
	kind = get_client_kind(buffer[4],buffer[5]);

	memcpy(user_name,&buffer[8],buffer[6]);//0-3 4-5 6 7
	user_name[buffer[6]] = '\0';
//	printf("user_name = %s\n",user_name);
//	printf("source_ip = %s\n",source_ip);
	if(PC == kind)
	{
		snprintf(cmd1,100,"update user set pc_ip=\"%s\" where user_name=\"%s\";",source_ip,user_name);
		snprintf(cmd2,100,"update user set pc_port=%d where user_name=\"%s\";",htons(p->p_sockaddr->sin_port),user_name);
		snprintf(cmd3,100,"select android_ip from user where user_name=\"%s\";",user_name);		
		snprintf(cmd4,100,"select android_port from user where user_name=\"%s\";",user_name);		
		printf("pc kind\n");
	}
	else if(ANDROID == kind)
	{
	
		snprintf(cmd1,100,"update user set android_ip=\"%s\" where user_name=\"%s\";",source_ip,user_name);
		snprintf(cmd2,100,"update user set android_port=%d where user_name=\"%s\";",htons(p->p_sockaddr->sin_port),user_name);

		snprintf(cmd3,100,"select pc_ip from user where user_name=\"%s\";",user_name);
		snprintf(cmd4,100,"select pc_port from user where user_name=\"%s\";",user_name);		
		printf("android kind\n");
	}
	else printf("clint kind error\n");	
//	printf("cmd1 = %s\n",cmd1);
//	printf("cmd2 = %s\n",cmd2);
//	printf("cmd3 = %s\n",cmd3);
//	printf("cmd4 = %s\n",cmd4);
	update_sql(cmd1);
	update_sql(cmd2);
	query_sql(cmd3,p->ip,lenth);
	p->port = query_sql_int(cmd4);
	printf("destination ip = %s , port = %d\n",p->ip,p->port);
	if(!strcmp(p->ip,"0.0.0.0")||p->port == 0)
	{
		printf("destination ip not login\n");
		return -1;
	}	
	return 0;
}

