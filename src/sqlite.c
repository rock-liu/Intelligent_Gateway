#include "../include/sqlite.h"
#include "../include/show.h"


int sqlite(char *buf)
{
	sqlite3* db;
	int ret  = sqlite3_open("/goods.db",&db);

	if(SQLITE_OK != ret)
	{
		fprintf(stderr,"can't open db %s\n",sqlite3_errmsg(db));
		sqlite3_close(db);
	}
	char * errmsg;
	char sql_cmd[256];
	printf("*********start insert*********\n");
	sprintf(sql_cmd,"insert into user values(NULL,\"%s\");",buf);
	printf("*********end insert***********\n");

	ret = sqlite3_exec(db,sql_cmd,NULL,NULL,&errmsg);
	if(SQLITE_OK != ret)
	{
		fprintf(stderr,"exec %s errmsg:%s\n",sql_cmd,errmsg);
		sqlite3_free(errmsg);
		sqlite3_close(db);
	}

		sqlite3_close(db);
	return 0;
}

int callback(void *arg, int col, char **result, char**title)
{
	static int num = 0;
	int i = 0;
	if (0 == num)
	{
		for (i = 0; i < col; i++)
		{
			printf("%s\t", title[i]);
		}
		printf("\n");
		num = 1;
	}
	
	for (i = 0; i < col; i++)
	{
		printf("%s\t",result[i]);
		char new_buf[256] = {0};
		strcpy(new_buf,result[i]);
		u32 * addr = init_fb();
		int sum_x = 100;
		int sum_y = 300;
		int value = 6;
		int len1 = strlen(new_buf);
		for (i = 0;i<len1;i++)
		{
			unsigned long  out = 0 ;
			int ret = enc_utf8_to_unicode_one(&new_buf[i],&out);
			char* temp = (char*)&out;
			sum_x += value; 
			show(addr,sum_x,sum_y,out,0x00FF1493);
		}
		sum_y += value;
	}
	printf("\n");
	return 0;
}

int sqlite_select(char *temp_buf)
{
	sqlite3* db;
	int ret  = sqlite3_open("/goods.db",&db);

	if(SQLITE_OK != ret)
	{
		fprintf(stderr,"can't open db %s\n",sqlite3_errmsg(db));
		sqlite3_close(db);
	}
	char * errmsg;
	char sql_cmd[256];
	printf("*********start select*********\n");
	sprintf(sql_cmd,"select %s from user;", temp_buf);

	ret = sqlite3_exec(db,sql_cmd,callback,NULL,&errmsg);
	if(SQLITE_OK != ret)
	{
		fprintf(stderr,"exec %s errmsg:%s\n",sql_cmd,errmsg);
		sqlite3_free(errmsg);
		sqlite3_close(db);
	}

	printf("*********end select***********\n");

	sqlite3_close(db);
	return 0;
}
