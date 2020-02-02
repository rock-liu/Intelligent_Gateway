#ifndef UPLOAD
#define UPLOAD


#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "../include/serial.h"
#define MAXLINE 4096
#define SERV_PORT 80
#define SERV_IP "183.230.40.33"

extern void* upload(char* upload_buf);

#endif
