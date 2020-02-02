/*************************************************************************
  > File Name: server.c
  > Author: yas
  > Mail: rage_yas@hotmail.com
  > Created Time: Thu 28 Jan 2016 11:25:10 AM CST
 ************************************************************************/
#include "../include/upload.h"

typedef struct sockaddr* (SA);

void* upload(char *upload_buf)
{

	struct sockaddr_in servaddr;
	int  connfd;
	int i = 0 ;
	char buf[MAXLINE];
	connfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(SERV_IP);
	servaddr.sin_port = htons(SERV_PORT);
	int ret =  connect(connfd,(SA)&servaddr,sizeof(servaddr));
	if(-1 == ret)
	{
		perror("connect");
		exit(1);
	}

	char temp[4096]={0};
	char* query[10]={NULL};
	char js_body[4096]={0};
	strtok(upload_buf," 开");
	char *temp1 = strtok(NULL, " ");
	char *temp2 = strtok(NULL, " 3");
	char *temp3 = strtok(NULL, " 10");
	char *temp4 = strtok(NULL, " ");
	printf("%s\n", temp1);
	printf("%s\n", temp2);
	printf("%s\n", temp3);
	printf("%s\n", temp4);
	sprintf(js_body,"{\"datastreams\": [{\"id\": \"stat\",\"datapoints\": [{\"value\": \"%s\"}, ]},\
			{\"id\": \"goods_name\",\"datapoints\": [{\"value\": \"%s\"}, ]},\
			{\"id\": \"price\",\"datapoints\": [{\"value\": \"%s\"}, ]},\
			{\"id\": \"left_number\",\"datapoints\": [{\"value\": \"%s\"}, ]}\
			]}",temp1,temp2,temp3,temp4);

	query[0]="POST /devices/576823356/datapoints HTTP/1.1\r\n";
	query[1]="api-key:R7FrgNa4QYmxODYPtWMI4UH=ytk=\r\n";
	query[2]="Host:api.heclouds.com\r\n";
	sprintf(temp,"Content-Length: %d\r\n\r\n",strlen(js_body));
	query[3]=temp;

	for(i = 0 ;i<4;i++)
	{
		ret =send(connfd,query[i],strlen(query[i]),0);
		printf("%d send len%d\n",i,ret);

	}
	ret = send(connfd,js_body,strlen(js_body),0);
	printf("%d send len%d\n",i,ret);
	recv(connfd,temp,4096,0);
	printf("hello world...\njs_len:%d\nrecv:%s \n",strlen(js_body),temp);

	return 0;
}
