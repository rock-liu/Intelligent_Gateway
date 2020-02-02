#include "../include/main.h"
#include "../include/serial.h"
#include "../include/show.h"
#include "../include/upload.h"


int main(int argc,char**argv)
{
	int fd =  IC_TTY_Init("/dev/ttySAC2",115200,8,'N',1);

	pthread_t tid1,tid2,tid3,tid4,tid5;
	pthread_create(&tid1,NULL,send_th,&fd);
	pthread_create(&tid2,NULL,recv_th,&fd);
	pthread_create(&tid3,NULL,framebuffer_show,&fd);
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	pthread_join(tid3,NULL);

	return 0;
}
