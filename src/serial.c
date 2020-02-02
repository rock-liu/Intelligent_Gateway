#include "../include/main.h"
#include "../include/serial.h"
#include "../include/show.h"
#include "../include/sqlite.h"
#include "../include/upload.h"

sem_t sem_send,sem_recv;

int IC_TTY_addToLibIc()
{
	printf("IC_TTY_addToLibIc");

	return 0;
}

int IC_TTY_Option_Set(int fd,int nSpeed,int nBits,char nEvent,int nStop)
{
	struct termios newtio,oldtio;

	if(tcgetattr(fd,&oldtio)!=0)
	{
		printf("SetupSerial 1");
		return -1;
	}

	bzero(&newtio,sizeof(newtio));

	newtio.c_cflag |= CLOCAL |CREAD;
	newtio.c_iflag &= ~CSIZE;

	switch(nBits)
	{
	case 5:
		newtio.c_cflag |= CS5;
		break;
	case 6:
		newtio.c_cflag |= CS6;
		break;
	case 7:
		newtio.c_cflag |= CS7;
		break;
	case 8:
		newtio.c_cflag |=CS8;
		break;
	default:
		newtio.c_cflag |=CS8;
		break;
	}

	switch(nEvent)
	{
	case 'o':
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |=PARODD;
		newtio.c_iflag |=(INPCK | ISTRIP);
		break;

	case 'E': 
		newtio.c_iflag |=(INPCK | ISTRIP);
		newtio.c_cflag |=PARENB;
		newtio.c_cflag &= ~PARODD;
		break;

	case 'N':
		newtio.c_cflag &= ~PARENB;
		break;
	default:
		newtio.c_cflag &= ~PARENB;
		break;

	}

	switch(nSpeed)
	{  
	case 2400:
		cfsetispeed(&newtio,B2400);
		cfsetospeed(&newtio,B2400);
		break;

	case 4800:
		cfsetispeed(&newtio,B4800);
		cfsetospeed(&newtio,B4800);
		break;

	case 9600:
		cfsetispeed(&newtio,B9600);
		cfsetospeed(&newtio,B9600);
		break;

	case 57600:
		cfsetispeed(&newtio,B57600);
		cfsetospeed(&newtio,B57600);
		break;

	case 115200:
		cfsetispeed(&newtio,B115200);
		cfsetospeed(&newtio,B115200);
		break;

	case 460800:
		cfsetispeed(&newtio,B460800);
		cfsetospeed(&newtio,B460800);
		break;

	default:
		cfsetispeed(&newtio,B9600);
		cfsetospeed(&newtio,B9600);
		break;
	}

	if(nStop == 1) 
	{
		newtio.c_cflag &= ~CSTOPB;
	}
	else if(nStop == 2)
	{
		newtio.c_cflag |= CSTOPB;
	}

	newtio.c_cc[VTIME] = 0; 
	newtio.c_cc[VMIN] = 1;

	tcflush(fd,TCIFLUSH);

	if((tcsetattr(fd,TCSANOW,&newtio))!=0)
	{
		printf("com set error");
		return -1;
	}

	return 0;
}

int IC_TTY_Init(const char* pTtyName,int nSpeed,int nBits,char nEvent,int nStop)
{
	if (NULL == pTtyName)
	{
		printf("pTtyName is NULL");
		return -1;
	}

	int nComFd = 0;

	nComFd = open(pTtyName, O_RDWR);
	if (nComFd <= 0 )
	{
		printf("Couldn't open %s", pTtyName);

		return -2;
	}
	else
	{
		printf("open %s success!\n", pTtyName);	
	}


	IC_TTY_Option_Set(nComFd,nSpeed,nBits,nEvent,nStop);

	//	fcntl( nComFd, F_SETFL, FNDELAY );

	return nComFd;
}

int IC_TTY_Destroy(int nComFd)
{
	if (nComFd > 0)
	{
		close(nComFd);
	}

	return 0;
}

void* send_th(void* arg)
{
	sem_init(&sem_send,0,1);
	sem_init(&sem_recv,0,0);


	int fd = *(int*)arg;
	char buf[256]={"1           开机          百岁山         3             10 "};
//	while (1)
//	{
		sem_wait(&sem_send);
		write(fd, buf, 256);
		sleep(2);
		sem_post(&sem_recv);
//	}

}

void* recv_th(void* arg)
{
	sem_init(&sem_send,0,1);
	sem_init(&sem_recv,0,0);


	int fd = *(int*)arg;
	char buf[256] = {0};
	int ret;
	int i = 0;
	int value = 7;
	fd_set rd_set,temp_set;
	FD_ZERO(&rd_set);
	FD_ZERO(&temp_set);
	FD_SET(fd,&temp_set);
	struct timeval tm;
	u32 * addr = init_fb();
	while(1)
	{
		sleep(1);
		sem_wait(&sem_recv);
		tm.tv_sec = 5;
		tm.tv_usec = 0 ;
		rd_set = temp_set;
		ret  = select(fd+1,&rd_set,NULL,NULL,&tm);
		if(ret >0)
		{
			ret = read(fd, buf,256);
			if (ret > 0)
			{
				printf("%s\n",buf);

				sqlite(buf);
				
				int len = strlen(buf);
				int sum = 110;
				for (i = 0;i<len;i++)
				{
					unsigned long  out = 0 ;
					int ret = enc_utf8_to_unicode_one(&buf[i],&out);
					char* temp = (char*)&out;
					sum += value; 
					show(addr,sum,250,out,0x00000000);
				}
				//upload(buf);
			}
		sem_post(&sem_send);
		}
		else if( 0 ==ret)
		{

			printf("time out ,no data recv...\n");
			framebuffer_show(&fd);
			char timeout_buf[]={"哦  ，链  接  出  了   点   问   题   !"};
			int len = strlen(timeout_buf);
			for (i = 0;i<len;i++)
			{
				int sum = 120;
				int value = 6;
				unsigned long  out = 0 ;
				int ret = enc_utf8_to_unicode_one(&timeout_buf[i],&out);
				char* temp = (char*)&out;
				sum += value; 
				show(addr,sum,250,out,0x00000000);
			}
		}
		else
		{
			printf("select error\n");
			return 0;
		}
	}

}

