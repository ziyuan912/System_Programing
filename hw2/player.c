/*b05902050黃子源*/
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char const *argv[])		//[host_id] [player_index] [random_key]
{
	char readfifoname[30],writefifoname[30],readbuf[30],writebuf[30];
	char index = argv[2][0] - 'A';
	int readfifo,writefifo,money[4],bidmoney;
	sprintf(readfifoname,"host%s_%s.FIFO",argv[1],argv[2]);
	sprintf(writefifoname,"host%s.FIFO",argv[1]);
	readfifo = open(readfifoname,O_RDWR);
	writefifo = open(writefifoname,O_RDWR);
	for(int i = 0;i < 10;i ++){
		read(readfifo,readbuf,sizeof(readbuf));
		sscanf(readbuf,"%d%d%d%d",&money[0],&money[1],&money[2],&money[3]);
		if(i % 4 == index)bidmoney = money[index];
		else bidmoney = 0;
		sprintf(writebuf,"%s %s %d\n",argv[2],argv[3],bidmoney);
		write(writefifo,writebuf,strlen(writebuf));
	}
	close(readfifo);
	close(writefifo);
	return 0;
}
