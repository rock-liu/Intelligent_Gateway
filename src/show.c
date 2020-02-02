#include "../include/main.h"
#include "../include/show.h"
#include "../include/serial.h"
#include "../include/queue.h"
#include "../include/sqlite.h"


u32 image_w;
u32 image_h;
u32 xres;
u32 yres;

u32* init_fb()                           /*屏幕初始化*/
{
	int fd = open("/dev/fb0", O_RDWR);
	if(fd < 0)
	{
		perror("open err \n");
		exit(1);
	}
	struct fb_var_screeninfo vinfo;
	ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);	
	xres = vinfo.xres;
	yres = vinfo.yres;
	u32 size = vinfo.xres*vinfo.yres*vinfo.bits_per_pixel/8;	
	u32* addr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(addr == (void*)-1)
	{
		perror("mmap err \n");
		exit(1);
	}
	close(fd);
	return addr;
}

void draw_point(u32* addr, u32 x, u32 y, u32 color)       /*画点函数*/
{
	if(x >= xres || y >= yres){
		return ;
	}
	*(addr + W * y + x) = color;		
}

void clean_screen(u32* addr, u32 color)          /*清屏函数*/
{
	u32 i, j;
	for(i=0; i<H; i++){
		for(j=0; j<W; j++)
			draw_point(addr, j, i, color);
	}
}

void draw_line(u32* addr, u32 x, u32 y, int len, u32 color)  /*划横线函数*/
{
	int i=0, j=0;
	for(i=0; i<3; i++){
		for(j=x; j< x + len; j++){
			draw_point(addr, j, y+i, color);
		}
	}	
}

void draw_sline(u32* addr, u32 x, u32 y, int len, u32 color)/*画竖线函数*/
{
	int i=0,j=0;
	for(i=0;i<3;++i)
	{
		for(j=y;j<y+len;j++)
		{
			draw_point(addr,x+i,j,color);
		}
	}
}

void draw_juxing(u32* addr, u32 x, u32 y, int long_,int weigh_ ,u32 color)/*画矩形框函数*/
{
	draw_line(addr,x,y,long_,color);
	draw_line(addr,x,y+weigh_,long_,color);
	draw_sline(addr,x,y,weigh_,color);
	draw_sline(addr,x+long_,y,weigh_,color);
}

int enc_get_utf8_size(const unsigned char pInput){
    unsigned char c = pInput;
    // 0xxxxxxx 返回0
    // 10xxxxxx 不存在
    // 110xxxxx 返回2
    // 1110xxxx 返回3
    // 11110xxx 返回4
    // 111110xx 返回5
    // 1111110x 返回6
    if(c< 0x80) return 0;
    if(c>=0x80 && c<0xC0) return -1;
    if(c>=0xC0 && c<0xE0) return 2;
    if(c>=0xE0 && c<0xF0) return 3;
    if(c>=0xF0 && c<0xF8) return 4;
    if(c>=0xF8 && c<0xFC) return 5;
    if(c>=0xFC) return 6;
    return -1;
}

int enc_utf8_to_unicode_one(const unsigned char* pInput, unsigned long *Unic)
{
	assert(pInput != NULL && Unic != NULL);

	// b1 表示UTF-8编码的pInput中的高字节, b2 表示次高字节, ...
	char b1, b2, b3, b4, b5, b6;

	*Unic = 0x0; // 把 *Unic 初始化为全零
	int utfbytes = enc_get_utf8_size(*pInput);
	//int utfbytes = 3;
	unsigned char *pOutput = (unsigned char *) Unic;

	switch ( utfbytes )
	{
	case 0:
		*pOutput     = *pInput;
		utfbytes    += 1;
		break;
	case 2:
		b1 = *pInput;
		b2 = *(pInput + 1);
		if ( (b2 & 0xE0) != 0x80 )
			return 0;
		*pOutput     = (b1 << 6) + (b2 & 0x3F);
		*(pOutput+1) = (b1 >> 2) & 0x07;
		break;
	case 3:
		b1 = *pInput;
		b2 = *(pInput + 1);
		b3 = *(pInput + 2);
		if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) )
			return 0;
		*pOutput     = (b2 << 6) + (b3 & 0x3F);
		*(pOutput+1) = (b1 << 4) + ((b2 >> 2) & 0x0F);
		break;
	case 4:
		b1 = *pInput;
		b2 = *(pInput + 1);
		b3 = *(pInput + 2);
		b4 = *(pInput + 3);
		if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
				|| ((b4 & 0xC0) != 0x80) )
			return 0;
		*pOutput     = (b3 << 6) + (b4 & 0x3F);
		*(pOutput+1) = (b2 << 4) + ((b3 >> 2) & 0x0F);
		*(pOutput+2) = ((b1 << 2) & 0x1C)  + ((b2 >> 4) & 0x03);
		break;
	case 5:
		b1 = *pInput;
		b2 = *(pInput + 1);
		b3 = *(pInput + 2);
		b4 = *(pInput + 3);
		b5 = *(pInput + 4);
		if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
				|| ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80) )
			return 0;
		*pOutput     = (b4 << 6) + (b5 & 0x3F);
		*(pOutput+1) = (b3 << 4) + ((b4 >> 2) & 0x0F);
		*(pOutput+2) = (b2 << 2) + ((b3 >> 4) & 0x03);
		*(pOutput+3) = (b1 << 6);
		break;
	case 6:
		b1 = *pInput;
		b2 = *(pInput + 1);
		b3 = *(pInput + 2);
		b4 = *(pInput + 3);
		b5 = *(pInput + 4);
		b6 = *(pInput + 5);
		if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
				|| ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80)
				|| ((b6 & 0xC0) != 0x80) )
			return 0;
		*pOutput     = (b5 << 6) + (b6 & 0x3F);
		*(pOutput+1) = (b5 << 4) + ((b6 >> 2) & 0x0F);
		*(pOutput+2) = (b3 << 2) + ((b4 >> 4) & 0x03);
		*(pOutput+3) = ((b1 << 6) & 0x40) + (b2 & 0x3F);
		break;
	default:
		return 0;
		break;
	}

	return utfbytes;
}

int enc_unicode_to_utf8_one(unsigned long unic, unsigned char *pOutput,
		int outSize)
{
	assert(pOutput != NULL);
	assert(outSize >= 6);

	if ( unic <= 0x0000007F )
	{
		// * U-00000000 - U-0000007F:  0xxxxxxx
		*pOutput     = (unic & 0x7F);
		return 1;
	}
	else if ( unic >= 0x00000080 && unic <= 0x000007FF )
	{
		// * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
		*(pOutput+1) = (unic & 0x3F) | 0x80;
		*pOutput     = ((unic >> 6) & 0x1F) | 0xC0;
		return 2;
	}
	else if ( unic >= 0x00000800 && unic <= 0x0000FFFF )
	{
		// * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
		*(pOutput+2) = (unic & 0x3F) | 0x80;
		*(pOutput+1) = ((unic >>  6) & 0x3F) | 0x80;
		*pOutput     = ((unic >> 12) & 0x0F) | 0xE0;
		return 3;
	}
	else if ( unic >= 0x00010000 && unic <= 0x001FFFFF )
	{
		// * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		*(pOutput+3) = (unic & 0x3F) | 0x80;
		*(pOutput+2) = ((unic >>  6) & 0x3F) | 0x80;
		*(pOutput+1) = ((unic >> 12) & 0x3F) | 0x80;
		*pOutput     = ((unic >> 18) & 0x07) | 0xF0;
		return 4;
	}
	else if ( unic >= 0x00200000 && unic <= 0x03FFFFFF )
	{
		// * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		*(pOutput+4) = (unic & 0x3F) | 0x80;
		*(pOutput+3) = ((unic >>  6) & 0x3F) | 0x80;
		*(pOutput+2) = ((unic >> 12) & 0x3F) | 0x80;
		*(pOutput+1) = ((unic >> 18) & 0x3F) | 0x80;
		*pOutput     = ((unic >> 24) & 0x03) | 0xF8;
		return 5;
	}
	else if ( unic >= 0x04000000 && unic <= 0x7FFFFFFF )
	{
		// * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		*(pOutput+5) = (unic & 0x3F) | 0x80;
		*(pOutput+4) = ((unic >>  6) & 0x3F) | 0x80;
		*(pOutput+3) = ((unic >> 12) & 0x3F) | 0x80;
		*(pOutput+2) = ((unic >> 18) & 0x3F) | 0x80;
		*(pOutput+1) = ((unic >> 24) & 0x3F) | 0x80;
		*pOutput     = ((unic >> 30) & 0x01) | 0xFC;
		return 6;
	}

	return 0;
}

void show(u32* addr,int heigh,int width,unsigned long zioff,u32 color)
{
	int fd = open("./ziku",O_RDONLY);
	if(-1== fd)
	{
		perror("ziku open error");
		exit(1);
	}
	//unsigned int off =  0x8f89*32;
	unsigned int off =  zioff*32;

	lseek(fd,off,SEEK_SET);
	char *data =malloc(32); //16*16/8
	bzero(data,32);
	int ret =read(fd,data,32);
	int i,j,k,num=0;;
	unsigned char temp=0;
	for(i=0;i<16;i++)
	{
		for(j=0;j<2;j++)
		{
			temp = data[num++];
			for(k=0;k<8;k++)
			{
				if(0x80&temp)
				{
					draw_point(addr,8*j+heigh+k,width+i,color);
				}
				else
				{
				//	printf(" ");
				}
				fflush(stdout);
				temp= temp<<1;
			}
		}
	}

	close(fd);

}
/*
QueLink*CreateQueue()
{
    QueLink* temp =  malloc(sizeof(QueLink));
    if(NULL == temp)
    {
        perror("CreateQueue malloc");
        exit(1);
    }
    temp->head =NULL;
    temp->tail = NULL;
    temp->len = 0;
    return temp;
}

int EmptyQueue(QueLink* q)
{
    return q->len == 0;
}

int EnterQueue(QueLink* q,DATATYPE *data)
{
    Node* newnode = malloc(sizeof(Node));
    if(NULL == newnode)
    {
        perror("EnterQueue malloc");
        return -1;
    }
    memcpy(&newnode->data,data,sizeof(DATATYPE));
    newnode->next = NULL;
    if(0 == q->len)
    {
        q->head = newnode;
        q->tail = newnode;

    }
    else
    {
        q->tail->next = newnode;
        q->tail = newnode;
    }
    q->len++;
    return 0;
}

int QuitQueue(QueLink* q,DATATYPE *data)
{
    if(NULL != data)
    {
        memcpy(data,&q->head->data,sizeof(DATATYPE));
    }
    Node* temp = q->head;
    if(q->head == q->tail)
    {
        q->tail =NULL;
    }
    q->head = q->head->next;
    free(temp);
    q->len--;

    return 0;
}

int GetHead(QueLink*q, DATATYPE *data1)
{
    memcpy(data1, &q->head->data, sizeof(DATATYPE));
    return 0;
}

int ClearQueue(QueLink* q)
{
    while (q->head != NULL)
    {
        QuitQueue(q, NULL);
    }
    q->len = 0;
    return 0;
}

int DestroyQueue(QueLink** q)
{
    while(!EmptyQueue(*q))
    {
        QuitQueue(*q, NULL);
    }

    free(*q);
    *q = NULL;
    return 0;
}
*/
int touchscreen()
{
	int fd_t = open("/dev/input/event0", O_RDWR);
	struct input_event ev;
	int x,y;

	//type EV_ABS 03  绝对坐标
	// code ABS_MT_POSITION_X 35 绝对坐标值的x值
	while(1)
	{
		read(fd_t,&ev,sizeof(ev));
		printf("type:0x%x code:ox%x value:0x%x\n",ev.type,ev.code,ev.value);

		if (3 == ev.type)
		{
			if (53 == ev.code)
			{
				if (ev.value > 592 && ev.value < 656)
				{
					sqlite_select("datials");
				}
			}

		}
	}
	return 0;
}

void* framebuffer_show(void* arg)
{
	u32 * addr = init_fb();
	clean_screen(addr, 0x00FAEBD7);
	draw_juxing(addr,80,100,100,100,0x00000000);
	draw_juxing(addr,180,100,100,100,0x00000000);
	draw_juxing(addr,280,100,100,100,0x00000000);
	draw_juxing(addr,380,100,100,100,0x00000000);
	draw_juxing(addr,480,100,100,100,0x00000000);
	draw_juxing(addr,580,100,100,100,0x00FF1493);

	char old_buf[]={"设备号      开关状态      商品名称    当前价格   当前剩余量"};
	int i = 0;
	int sum = 100;
	int value = 6;
	int len = strlen(old_buf);
	for (i = 0;i<len;i++)
	{
		unsigned long  out = 0 ;
		int ret = enc_utf8_to_unicode_one(&old_buf[i],&out);
		//printf("utf8-ut ret is %d ,buf is %d\n",ret,sizeof(buf));
		char* temp = (char*)&out;
		sum += value; 
		show(addr,sum,150,out,0x00000000);
	}

	char new_buf[]={"历史查询"};
	int sum_x = 600;
	int len1 = strlen(new_buf);
	for (i = 0;i<len1;i++)
	{
		unsigned long  out = 0 ;
		int ret = enc_utf8_to_unicode_one(&new_buf[i],&out);
		char* temp = (char*)&out;
		sum_x += 6;
		show(addr,sum_x,150,out,0x00FF1493);
	}
	touchscreen();
}
