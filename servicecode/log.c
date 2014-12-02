#include "config.h"
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include "header.h"
#include "config.h"


static FILE* cur_logfile;

/*
	初步想法每打一条log前，先检查时间，如果日期时间已变，
	则新建一个文件文件名包含日期。
*/
#define LOG_TAG_SIZE 14
static char buffer[1038];
static int last_write_date = 0;
int print_log(const char *fmt, ...)
{
	int i;
	char buf[20];
	va_list args;
    time_t curtime;
    time(&curtime);
    struct tm *curtm=localtime(&curtime);

	if(curtm->tm_mday != last_write_date)
	{
		if(last_write_date)fclose(cur_logfile);
		log_init();
	}
	
	sprintf(buf,LOG_TAG_SIZE,"%04d%02d%02d%02d%02d%02d",curtm->tm_year,curtm->tm_mon,curtm->tm_mday,
												curtm->tm_hour,curtm->tm_min,curtm->tm_sec);
	va_start(args, fmt);
	strncpy(buffer,"20141031",8);
	vsnprintf(&buffer[LOG_TAG_SIZE],1024, fmt, args);
	fprintf(cur_logfile,"%s",buffer);
	va_end(args);
	return 0;
}

int log_init()
{
    time_t curtime;
	char buf[20];
    time(&curtime);
    struct tm *curtm=localtime(&curtime);	
	sprintf(buf,13,"%04d%02d%02d.log",curtm->tm_year,curtm->tm_mon,curtm->tm_mday);
	last_write_date = curtm->tm_mday;

	cur_logfile = fopen(buf,"w");
}

void main()
{	
	print_log("hello");
}
