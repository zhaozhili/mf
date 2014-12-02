#include "config.h"
#include <string.h>
#include "header.h"
#define MAX_USERNAME_LENGTH 20
#define MAX_USERPASSWD_LENGTH 16

#define MACHINE_ID_LENGTH 16
//规定传输格式:程序包头(4)+  标志终端类型(1) + 操作类型(1)  
//  操作所需参数个数(1)+ 每个参数的长度(参数个数) +数据
int do_login_cmd(char* buf,struct tcp_param *p)//login data order : 用户名长度用户名，密码长度 密码
{
	int username_size = buf[1];
	int passwd_size = buf[2];	
	char cmd[100];
	
	char user_name[20];
	char user_passwd[20];
	char service_passwd[20];

	char datalength[2];
	int cnt;	
	if(buf[0] !=2 )return -1;
	memcpy(user_name,&buf[3],username_size);
	user_name[username_size] = '\0';
	memcpy(user_passwd,&buf[3+username_size],passwd_size);
	user_passwd[passwd_size] = '\0';
	snprintf(cmd,60,"select pass_wd from user where user_name=\"%s\";",user_name);
	printf("cmd=%s\n",cmd);
	if((cnt = query_sql(cmd,service_passwd,datalength)) == 1)
	{
		printf("user passwd %s\n",user_passwd);
		printf("service_pass %s\n",service_passwd);
		if(!strcmp(user_passwd,service_passwd))
		{
			cnt = send_ok_toclient(p->fd,NULL,0);
/*			if(cnt)printf("send ok error\n");
			
			if(p->kind == PC)
				snprintf(cmd,60,"update user set pc_ip=\"%s\" where user_name=\"%s\";",p->ip,user_name);
			else	
				snprintf(cmd,60,"update user set android_ip=\"%s\" where user_name=\"%s\";",p->ip,user_name);
			
			printf("update ip cmd %s",cmd);*/
			return cnt;
		}
	}
	else 
		printf("%s %d :sql return data count incorrect.cnt=  %d\n",__func__,__LINE__,cnt);
	return -1;
	
}

int do_change_passwd_cmd(char* buf,struct tcp_param *p)//login data order : 用户名长度用户名，密码长度 密码
{
	int username_size = buf[1];
	int passwd_size = buf[2];
	int new_passwd_size = buf[3];
	char cmd[100];
	
	char user_name[20];
	char user_passwd[20];
	char service_passwd[20];

	char datalength[2];
	int cnt;	
	//if(buf[0] !=2 )return -1;
//	printf("do change passwd\n");
	memcpy(user_name,&buf[4],username_size);
	user_name[username_size] = '\0';
	memcpy(user_passwd,&buf[4+username_size],passwd_size);
	user_passwd[passwd_size] = '\0';
	snprintf(cmd,100,"select pass_wd from user where user_name=\"%s\";",user_name);
	printf("cmd=%s\n",cmd);
	if((cnt = query_sql(cmd,service_passwd,datalength)) == 1)
	{
		printf("username %s\n",user_passwd);
		printf("service_pass %s\n",service_passwd);
		if(!strcmp(user_passwd,service_passwd))
		{
			printf("here we can change passwd\n");
			memcpy(user_passwd,&buf[4+username_size+passwd_size],new_passwd_size);//put new passwd to there.
			snprintf(cmd,100,"update user set pass_wd=\"%s\" where user_name=\"%s\";",user_passwd,user_name);
			printf("new pass cmd = %s\n",cmd);
			cnt = update_sql(cmd);
			if(cnt == 0)cnt = send_ok_toclient(p->fd,NULL,0);
			if(cnt)printf("send ok error\n");
			
		}
	}
	return cnt;
}	
int do_get_machinid_cmd(char* buf,int fd)
{

	int username_size = buf[1];
	int cnt;

	char cmd[60];
	char user_name[20];
	char machine_id[MACHINE_ID_LENGTH+2];
	char datalength[2];


	memcpy(user_name,&buf[2],username_size);
	user_name[username_size] = '\0';
	printf("username=%s\n",user_name);
	snprintf(cmd,60,"select machine_id from user where user_name=\"%s\";",user_name);
	printf("cmd=%s\n",cmd);
	cnt = query_sql(cmd,machine_id,datalength);

	if(cnt)
	{
		printf("machine ID %s  id length = %d\n",machine_id,strlen(machine_id));

		if(strlen(machine_id) == MACHINE_ID_LENGTH)
		{
			return send_ok_toclient(fd,machine_id,MACHINE_ID_LENGTH);
		}
	}
	else 
		printf("%s %d :sql return data count incorrect. cnt=  %d\n",__func__,__LINE__,cnt);
	return -1;


}
/*
int do_get_pcclinet_ip_cmd(char* user_name,char *buf)
{

	int username_size = buf[1];
	char datalength[2];
	int cnt;

	char cmd[60];

	snprintf(cmd,60,"select pc_ip from user where user_name=\"%s\";",user_name);
	printf("cmd=%s\n",cmd);
	
	return  query_sql(cmd,buf,datalength);
}

int do_start_transform_data_cmd(char* buf,int fd)
{

	int username_size = buf[1];
	int data_size = buf[2];
	int cnt;

	char cmd[60];
	char user_name[20];

	char pcip_buf[20];
	char androidip_buf[20];

	memcpy(user_name,&buf[2],username_size);
	user_name[username_size] = '\0';
	printf("username=%s\n",user_name);
	snprintf(cmd,60,"select PC_IP from user where user_name=\"%s\";",user_name);
	printf("cmd=%s\n",cmd);
	cnt = query_sql(cmd,machine_id,datalength);

	if(cnt)
	{
		printf("machine ID %s  id length = %d\n",machine_id,strlen(machine_id));

		if(strlen(machine_id) == MACHINE_ID_LENGTH)
		{
			return send_ok_toclient(fd,machine_id,MACHINE_ID_LENGTH);
		}
	}
	else 
		printf("%s %d :sql return data count incorrect. cnt=  %d\n",__func__,__LINE__,cnt);
	return -1;


}
*/
int do_register_account(char* buf)
{
	int data_size = buf[0] + (buf[1] << 8);
}
int do_register_machineid_cmd(char* buf)
{/*
	int param_cnt = buf[0];
	char cmd[60];

	if(cnt != 0)
	{
		return -1;
	}
	
	snprintf(cmd,60,"select machine_id from user where user_name=\"%s\";",user_name);
	printf("cmd=%s\n",cmd);*/
	return 0;

}


int handle_cmd_data(char buf[],struct tcp_param *p)
{
	int ret = 0;
	printf("cmd number = %d\n",buf[0]);
	switch(buf[0])
	{
		case LOG_IN:
			ret = do_login_cmd(&buf[1],p);
		break;
		case 3:
			ret = do_change_passwd_cmd(&buf[1],p);	
		break;
		case GET_MACHINE_ID:			
			ret = do_get_machinid_cmd(&buf[1],p->fd);
		break;
		case REGISTER_MACHINE_ID:			
			ret = do_register_machineid_cmd(&buf[1]);
		break;
	//	case REGISTER_ACCOUNT:
	//		ret = do_register_account(&buf[1]);
	//	break;

	       default : ret = -1;
	}
	return ret;
}
/*
void main()
{

}
*/
