#include "config.h"
#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
//this file is to handle drill net hole and handle heart package
//
#define UDP_PORT_NUMBER 13335
#define UDP_MESSAGEBUF_COUNT 100
#define MAXDATASIZE 1024

#define CMD_KEEP_TOUCH 0x61
#define CMD_DRILL_HOLE  0x60
#define DRILL_CAN_START 0x65
int addr_len;	
struct udpdata_handle_param
{
	char ip[20];
	int port;
	struct sockaddr_in *p_sockaddr;
	char user_name[20];
	enum client_kind kind;
};
 int start_drill_hole(void)	
 {	
	 int mysock,len;  
	 struct sockaddr_in addr;  
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
	 if(bind(mysock,(struct sockaddr *)(&addr),sizeof(addr))<0)	
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
		 len= recvfrom (mysock,msg,sizeof(msg),0,(struct sockaddr *)&addr,&addr_len);/*?????*/  
		//printf("message from : %s port=%d\n",inet_ntoa(addr.sin_addr),htons(addr.sin_port));
		 p.p_sockaddr = &addr;

		if(check_data_head(&msg[0]))
		{	
			printf("__func__,__LINE__ pakage head wrong\n");
			return -1;		
		}
		p.kind = get_client_kind(msg[4],msg[5]);
		memcpy(p.user_name,&msg[8],msg[7]);//0-3 4-5 6 package kind(drill or heart) 7 username length 8 data
		p.user_name[msg[7]] = '\0';
		if(msg[6] == CMD_KEEP_TOUCH)
		{
			do_heart_jump_handle(&p,mysock);
		}
		else if(msg[6] == CMD_DRILL_HOLE)
		{
		/*	if(!query_acount_log_state(&p))
			{
				printf("acount not login and return \n");
				return ;
			}*/
			do_drill_handle(&p,mysock,msg);
		}
	 }	  
 }	
int query_acount_log_state(struct udpdata_handle_param *p)
{
	 char cmd3[100];
	 char lenth[2];
	struct timeval tpnow;
	int last_touch;
	
	gettimeofday(&tpnow,NULL);
	if(PC == p->kind)
	{
		snprintf(cmd3,100,"select android_login_status from user where user_name=\"%s\";",p->user_name);			
	}
	else if(ANDROID == p->kind)
	{
		snprintf(cmd3,100,"select pc_login_status from user where user_name=\"%s\";",p->user_name);	
	}
	else 
	{
		printf("clint kind error\n");	
		return -1;
	}
	printf("cmd1 = %s\n",cmd3);
	last_touch = query_sql_int(cmd3);
	if((tpnow.tv_sec - last_touch)<10 && (tpnow.tv_sec - last_touch)>0)return 0;
	else return -1; 
}
 int do_drill_handle(struct udpdata_handle_param *p ,int sockfd,char *msg)
{
	char lenth[2];
	char cmd3[100];
	char cmd4[100];
	char temp1[30];//save source ip and port to destination
	char temp2[10];
	char user_name[20];
	//char *source_ip = (char *)inet_ntoa(p->p_sockaddr->sin_addr);
	enum client_kind kind;
	struct sockaddr_in to_addr;
	ssize_t len = 0;

	int i;
	if(PC == p->kind)
	{
		snprintf(cmd3,100,"select android_ip from user where user_name=\"%s\";",p->user_name);		
		snprintf(cmd4,100,"select android_port from user where user_name=\"%s\";",p->user_name);		
	}
	else if(ANDROID == p->kind)
	{
		snprintf(cmd3,100,"select pc_ip from user where user_name=\"%s\";",p->user_name);
		snprintf(cmd4,100,"select pc_port from user where user_name=\"%s\";",p->user_name);		
	}
	else 
	{
		printf("clint kind error\n");	
		return -1;
	}
	printf("cmd3 = %s\n",cmd3);
	printf("cmd4 = %s\n",cmd4);
	query_sql(cmd3,p->ip,lenth);
	p->port = query_sql_int(cmd4);
	printf("destination ip = %s , port = %d\n",p->ip,p->port);
	if(!strcmp(p->ip,"0.0.0.0")||p->port == 0)
	{
		printf("destination ip not login\n");
		return -1;
	}
	
	temp1[0] = (char)strlen(p->ip);
	memcpy(&temp1[1],p->ip,temp1[0]);
	printf("a ip = %s,%s\n",p->ip,&temp1[1]);

	sprintf(temp2, "%d", p->port);
	temp1[temp1[0]+1] = (char)strlen(temp2);//ip str length + str + port str length

	memcpy(&msg[8+msg[7]],temp1,(2+temp1[0]));
	memcpy(&msg[10+msg[7]+temp1[0]],temp2,temp1[temp1[0]+1]);
	msg[6] = DRILL_CAN_START;//to android
	
	for(i=0;i<40;i++)printf("msg[%d] = %x:%c\n",i,msg[i],msg[i]);
//	printf("ip first data = %c\n",msg[10+msg[7]]);
	 len =  sendto(sockfd,msg,40,0,(struct sockaddr *)p->p_sockaddr,addr_len); /*send ack to where it come*/	
	printf("len = %d\n",(int)len);

	msg[6] = CMD_DRILL_HOLE;//to pc
	bzero(&to_addr,sizeof(to_addr)); 
	to_addr = *p->p_sockaddr; 
	to_addr.sin_port=htons(p->port);  
 	inet_aton(p->ip,&to_addr.sin_addr);
	len =  sendto(sockfd,msg,40,0,(struct sockaddr *)&to_addr,addr_len); /*send drill cmd to destination*/	
	printf("len = %d\n",(int)len);
	return 0;
}
 int do_heart_jump_handle(struct udpdata_handle_param *p ,int sockfd)
{
	char cmd1[100];
	char cmd2[100];
	char cmd3[100];
	unsigned long ip_addr =  inet_ntoa(p->p_sockaddr->sin_addr);
	char source_ip [20];
	struct sockaddr_in to;
	struct sockaddr_in to_addr;  
	struct timeval tpnow;
	
	gettimeofday(&tpnow,NULL);
	 memcpy(source_ip,(char*)ip_addr,20);
	if(PC == p->kind)
	{
		snprintf(cmd1,100,"update user set pc_ip=\"%s\" where user_name=\"%s\";",source_ip,p->user_name);
		snprintf(cmd2,100,"update user set pc_port=%d where user_name=\"%s\";",htons(p->p_sockaddr->sin_port),p->user_name);
		snprintf(cmd2,100,"update user set pc_login_status=%d where user_name=\"%s\";",(int)tpnow.tv_sec,p->user_name);
	}
	else if(ANDROID == p->kind)
	{
		snprintf(cmd1,100,"update user set android_ip=\"%s\" where user_name=\"%s\";",source_ip,p->user_name);
		snprintf(cmd2,100,"update user set android_port=%d where user_name=\"%s\";",htons(p->p_sockaddr->sin_port),p->user_name);
		snprintf(cmd2,100,"update user set android_login_status=%d where user_name=\"%s\";",(int)tpnow.tv_sec,p->user_name);
	}
	else 
	{
		printf("clint kind error\n");	
		return -1;
	}
	printf("cmd1 = %s\n",cmd1);
	printf("cmd2 = %s\n",cmd2);
	printf("cmd3 = %s\n",cmd3);
	update_sql(cmd1);
	update_sql(cmd2);
	update_sql(cmd3);
	return 0;
}
/*
int main()
{
	start_drill_hole();
}*/
