#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "config.h"
#include "header.h"

int main(int argc,char *argv[])
{
	int sockfd;
	pid_t pid;
	struct sockaddr_in server_addr;
//	start_drill_hole();
	sockfd = net_init(&server_addr);
	if(sockfd<0)
	{
		printf("net init error.\n");
		return -1;
	}	
	pid = fork();
	if(pid<0)
	{
		printf("fork error\n");
		return -1;
	}
	if(pid==0)start_net(sockfd,&server_addr);
	else start_udp_transform();
	return 0;
}
