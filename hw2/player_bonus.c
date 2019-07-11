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
	sprintf(readfifoname,"host%d_%c.FIFO",argv[1],argv[2]);
	sprintf(writefifoname,"host%d.FIFO",argv[1]);
	readfifo = open(readfifoname,O_RDWR);
	writefifo = open(writefifoname,O_RDWR);
	for(int i = 0;i < 10;i ++){
		read(readfifo,readbuf,sizeof(readbuf));
		int count = 0,max = -1;
		sscanf(readbuf,"%d%d%d%d",&money[0],&money[1],&money[2],&money[3]);
		for(int j = 0;j < 4;j ++){
			if(j != index && money[j] < money[index]){
				count ++;
				if(max < money[j])max = money[j];
			}
		}
		if(count == 3){
			bidmoney = max;
		}
		else{
			bidmoney = 3*money[index]/4 + 1;
		}
		if(bidmoney > money[index])bidmoney = money[index];
		sprintf(writebuf,"%c %d %d\n",argv[2],argv[3],bidmoney);
		write(writefifo,writebuf,strlen(writebuf));
	}
	close(readfifo);
	close(writefifo);
	return 0;
}