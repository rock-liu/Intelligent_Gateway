#ifndef SERIAL
#define SERIAL

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <semaphore.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

extern int IC_TTY_addToLibIc();
extern int IC_TTY_Option_Set(int fd,int nSpeed,int nBits,char nEvent,int nStop);
extern int IC_TTY_Init(const char* pTtyName,int nSpeed,int nBits,char nEvent,int nStop);
extern int IC_TTY_Destroy(int nComFd);
extern void* send_th(void* arg);
extern void* recv_th(void* arg);

#endif // HEAD

