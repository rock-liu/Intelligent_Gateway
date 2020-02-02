#ifndef SHOW
#define SHOW

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
#define	rgb888(r,g,b)	(((r&0xff)<<16)|((g & 0xff)<<8)|(b&0xff))

#define	W	800
#define	H	480
typedef unsigned long	u32;
typedef unsigned char	u8;


extern int enc_get_utf8_size(const unsigned char pInput);
extern int enc_utf8_to_unicode_one(const unsigned char* pInput, unsigned long *Unic);
extern int enc_unicode_to_utf8_one(unsigned long unic, unsigned char *pOutput,int outSize);
extern void show(u32* addr,int heigh,int width,unsigned long zioff,u32 color);
extern u32* init_fb();                          /*屏幕初始化*/
extern void clean_screen(u32* addr, u32 color);          /*清屏函数*/
extern void draw_point(u32* addr, u32 x, u32 y, u32 color);       /*画点函数*/
extern void draw_line(u32* addr, u32 x, u32 y, int len, u32 color);  /*划横线函数*/
extern void draw_sline(u32* addr, u32 x, u32 y, int len, u32 color);/*画竖线函数*/
extern void draw_juxing(u32* addr, u32 x, u32 y, int long_,int weigh_ ,u32 color);/*画矩形框函数*/
extern int touchscreen();
extern void* framebuffer_show(void* arg);

#endif
