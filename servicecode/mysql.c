#include <string.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <mysql/mysql.h>
#include "config.h"
#include "header.h"

#pragma comment(lib,"libmysql") 

void exe_sql(char* sql) 
{ 

	MYSQL my_connection;
	int res;
	mysql_init(&my_connection); 
	if (mysql_real_connect(&my_connection, HOST, USERNAME, PASSWORD, 
	DATABASE, 0, NULL, CLIENT_FOUND_ROWS)) {

		printf("data base connect success\n"); 
		mysql_query(&my_connection, "set names utf8"); 
	//	res = mysql_query(&my_connection, "use sample_db"); 
		res = mysql_query(&my_connection, sql); 
		if (res) {
			printf("Error:  mysql_query\n"); 
			mysql_close(&my_connection); 
		} else {
			printf("%d line changed\n", 
			(int)mysql_affected_rows(&my_connection)); 
			mysql_close(&my_connection); 
		} 
	}
	else
	{ 
		printf("failed to database\n"); 
	} 
} 
int query_sql_int(char* sql_cmd) 
{ 
	MYSQL my_connection;
	int res;
	MYSQL_RES *res_ptr = NULL;
	MYSQL_FIELD *field;
	MYSQL_ROW result_row; 
	int column; 
	int i;
	int num_fields;
	int size = 0;
	mysql_init(&my_connection); 
	if (mysql_real_connect(&my_connection, HOST, USERNAME, PASSWORD, 
								DATABASE, 0, NULL, CLIENT_FOUND_ROWS)) 
	{
		mysql_query(&my_connection, "set names utf8"); 
		res = mysql_query(&my_connection, sql_cmd); 
		if (res) 
		{
			printf("Error mysql_query !\n"); 
			mysql_close(&my_connection);
			return -1; 
		} 
		else 
		{
			res_ptr = mysql_store_result(&my_connection);
			res = *((int *)res_ptr); 
			if(!res)
			{
				printf("No acount infomation res_ptr=%d\n",*res_ptr);
				return -1;
			}
			column = mysql_num_fields(res_ptr);
			for (i = 0; i < column; i++)
			{
				result_row = mysql_fetch_row(res_ptr);
		//		printf("mysql:get a number = %s\n",result_row[i]); 
			}
		}
		mysql_close(&my_connection); 
	}	
	return atoi(result_row[0]); 
} 
int query_sql(char* sql_cmd,char databuf[],char get_data_size[]) 
{ 
	MYSQL my_connection;
	int res;
	MYSQL_RES *res_ptr = NULL;
	MYSQL_FIELD *field;
	MYSQL_ROW result_row; 
	int column; 
	int i;
	int num_fields;
	int temp,size = 0;
	mysql_init(&my_connection); 
	if (mysql_real_connect(&my_connection, HOST, USERNAME, PASSWORD, 
								DATABASE, 0, NULL, CLIENT_FOUND_ROWS)) 
	{
		mysql_query(&my_connection, "set names utf8"); 
		res = mysql_query(&my_connection, sql_cmd); 
		if (res) 
		{
			printf("Error mysql_query !\n"); 
			mysql_close(&my_connection);
			return -1; 
		} 
		else 
		{
			res_ptr = mysql_store_result(&my_connection);
			res = *((int *)res_ptr); 
			if(!res)
			{
				printf("No acount infomation res_ptr=%d\n",*res_ptr);
				return -1;
			}
			column = mysql_num_fields(res_ptr);
			for (i = 0; i < column; i++)
			{
				result_row = mysql_fetch_row(res_ptr);
				temp = strlen(result_row[i]);
				memcpy(&databuf[size], result_row[i],temp);
				databuf[size+temp] = '\0';
			//	printf("mysql:%s\n",&databuf[size]); 
				size += temp;
				get_data_size[i] = temp;
			}
		}
		mysql_close(&my_connection); 
	}	
	return i;  
} 
int update_sql(char* sql_cmd) 
{ 
	int res;
	MYSQL my_connection;
	MYSQL_RES *res_ptr = NULL;
	MYSQL_FIELD *field;
	MYSQL_ROW result_row; 
	mysql_init(&my_connection); 
	if (mysql_real_connect(&my_connection, HOST, USERNAME, PASSWORD, 
								DATABASE, 0, NULL, CLIENT_FOUND_ROWS)) 
	{
		mysql_query(&my_connection, "set names utf8"); 
		res = mysql_query(&my_connection, sql_cmd); 
		if (res) 
		{
			printf("Error mysql_query !\n"); 
			mysql_close(&my_connection);
			return -1; 
		} 
		else 
		{
		//	printf("%s update success\n",sql_cmd);
		}
		mysql_close(&my_connection); 
	}	
	return 0;  
} 

