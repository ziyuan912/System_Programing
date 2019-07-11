/*b05902050黃子源*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char const *argv[])
{
	char buf[30],fifoname[5][30],keybuf[4][30];
	int fd[2],infifofd[5],outfifofd,pid[4];
	pid_t child[4];
	
	sprintf(fifoname[4],"host%s.FIFO",argv[1]);
	mkfifo(fifoname[4],0700);
	outfifofd = open(fifoname[4],O_RDWR);
	for(int i = 0;i < 4;i ++){
		memset(fifoname[i],0,sizeof(fifoname));
		sprintf(fifoname[i],"host%s_%c.FIFO",argv[1],i + 'A');
		mkfifo(fifoname[i],0700);
		infifofd[i] = open(fifoname[i],O_RDWR);
	}
	while(1){
		int pmoney[5] = {0},pbid[5] = {0},pscore[5] = {0},pkey[5] = {0},pnum[5] = {0},pbid2[5] = {0},prank[5] = {0};
		memset(buf,0,sizeof(buf));
		read(STDIN_FILENO,buf,sizeof(buf));
		sscanf(buf,"%d %d %d %d",&pid[0],&pid[1],&pid[2],&pid[3]);
		if(pid[0] == -1 && pid[1] == -1 && pid[2] == -1 && pid[3] == -1)
			break;
		srand((unsigned)time(NULL));
		for (int i = 0; i < 4; i++)
		{
			pkey[i] = rand() % 65536;
			memset(keybuf[i],0,sizeof(keybuf[i]));
			sprintf(keybuf[i],"%d",pkey[i]);
		}
		for(int i = 0;i < 4;i ++){
			if((child[i] = fork()) == 0){
				char pindex[30] = {0};
				pindex[0] = i + 'A';
				execlp("./player","./player",argv[1],pindex,keybuf[i],(char*) 0);
			}
		}
		for(int i = 0;i < 10;i ++){
			char pmessege[50] = {0},preturn[50] = {0},reindex[4][10] = {{0}};
			int pread[4] = {0},winpid = -1,psort[5] = {0};
			for(int j = 0;j < 4;j ++){
				pmoney[j] += 1000 - pnum[j]*pbid[j];
			}
			memset(pmessege,0,sizeof(pmessege));
			sprintf(pmessege,"%d %d %d %d\n",pmoney[0],pmoney[1],pmoney[2],pmoney[3]);
			for(int j = 0;j < 4;j ++){
				write(infifofd[j],pmessege,strlen(pmessege));
			}
			while(1){
				int count  = 0,key[4],remoney[4];
				memset(preturn,0,sizeof(preturn));
				memset(reindex,0,sizeof(reindex));
				read(outfifofd,preturn,sizeof(preturn));
				for(int j = 0;j < strlen(preturn);j ++){
					if(preturn[j] == '\n')count ++;
				}
				if(count == 1)
					sscanf(preturn, "%s %d %d\n", reindex[0], &key[0], &remoney[0]);
				else if(count == 2)
					sscanf(preturn, "%s %d %d\n%s %d %d\n", reindex[0], &key[0], &remoney[0],reindex[1], &key[1], &remoney[1]);
				else if(count == 3)
					sscanf(preturn, "%s %d %d\n%s %d %d\n%s %d %d\n", reindex[0], &key[0], &remoney[0],reindex[1], &key[1], &remoney[1],reindex[2], &key[2], &remoney[2]);
				else if(count == 4)
					sscanf(preturn, "%s %d %d\n%s %d %d\n%s %d %d\n%s %d %d\n", reindex[0], &key[0], &remoney[0],reindex[1], &key[1], &remoney[1],reindex[2], &key[2], &remoney[2],reindex[3], &key[3], &remoney[3]);
				for(int j = 1;j <= count;j ++){
					for(int k = 0;k < 4;k ++){
						if(reindex[j-1][0] == k + 'A' && key[j-1] == pkey[k]){
							pbid[k] = remoney[j-1];
							pbid2[k] = pbid[k];
							pread[k] = 1;
						}
					}
				}
				if(pread[0] == 1 && pread[1] == 1 && pread[2] == 1 && pread[3] == 1)
					break;
			}
			//for(int j = 0;j < 4;j ++)
			//	fprintf(stderr, "%c = %d\n",j + 'A',pmoney[j]);
			for(int j = 0;j < 4;j ++){
				int max = -1;
				for(int k = 0;k < 4;k ++){
					if(pbid2[k] > max)
						max = k;
				}
				psort[j] = max;
				pbid2[max] = -1;
			}
			for(int j = 0;j < 4;j ++){
				if(j == 0 && pbid[psort[j]] != pbid[psort[j + 1]]){
					winpid = psort[j];
					break;
				}
				else if(pbid[psort[j]] != pbid[psort[j + 1]] && pbid[psort[j]] != pbid[psort[j - 1]] && j != 3){
					winpid = psort[j];
					break;
				}
				else if(j == 3 && pbid[psort[j]] != pbid[psort[j - 1]])
					winpid = psort[j];
			}
			for(int j = 0;j < 4;j ++){
				if(winpid != -1 && j == winpid){
					pnum[winpid] = 1;
				}
				else
					pnum[j] = 0;
			}
			pscore[winpid] ++;
		}
		for(int i = 0;i < 4;i ++){
			for(int j = 0;j < 4;j ++){
				if(pscore[i] < pscore[j]){
					prank[i] ++;
				}
			}
		}
		memset(buf,0,sizeof(buf));
		sprintf(buf,"%d %d\n%d %d\n%d %d\n%d %d\n",pid[0],prank[0] + 1,pid[1],prank[1] + 1,pid[2],prank[2] + 1,pid[3],prank[3] + 1);
		write(STDOUT_FILENO,buf,strlen(buf));

		for(int i = 0;i < 4;i ++){
			waitpid(child[i],NULL,0);
		}
	}
	for(int i = 0;i < 4;i ++){
		close(infifofd[i]);
		unlink(fifoname[i]);
	}
	memset(fifoname[4],0,sizeof(fifoname[4]));
	sprintf(fifoname[4],"host%s.FIFO",argv[1]);
	unlink(fifoname[4]);
	close(outfifofd);
	return 0;
}




