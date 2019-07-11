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
#include <sys/time.h>

int main(int argc, char const *argv[])
{
	int fdin[13][2],fdout[13][2],index,idlehost[13];
	int playnum,maxfd,pscore[22] = {0},playid[5],prank[5],count = 0,num;
	pid_t pid[13];
	char givehost[50],buf[50];
	struct timeval timout;
	timout.tv_sec = 0;
	timout.tv_usec = 0;
	fd_set master,readfd;
	maxfd = getdtablesize();
	sscanf(argv[1],"%d",&index);
	sscanf(argv[2],"%d",&playnum);
	for(int i = 0;i < index;i ++){
		pipe(fdin[i]);
		pipe(fdout[i]);
		idlehost[i] = 1;
	}

	for(int i = 0;i < index;i ++){
		if((pid[i] = fork()) == 0){
			char hostnum[10] = {0};
			close(fdin[i][1]);
			close(fdout[i][0]);
			dup2(fdin[i][0],STDIN_FILENO);
			dup2(fdout[i][1],STDOUT_FILENO);
			close(fdout[i][1]);
			close(fdin[i][0]);
			memset(hostnum,0,sizeof(hostnum));
			sprintf(hostnum,"%d",i + 1);
			execlp("./host","./host",hostnum, (char*)0);
		}
		else{
			close(fdout[i][1]);
			close(fdin[i][0]);
		}
	}
	FD_ZERO(&master);
	for(int i = 0;i < index;i ++){
		FD_SET(fdout[i][0],&master);
	}
	int freehost = 0;
	for(int i1 = 0;i1 < playnum;i1 ++){
		for(int i2 = i1 + 1;i2 < playnum;i2 ++){
			for(int i3 = i2 + 1;i3 < playnum;i3 ++){
				for(int i4 = i3 + 1;i4 < playnum;i4 ++){
					int busy = 1;
					memset(givehost,0,sizeof(givehost));
					sprintf(givehost,"%d %d %d %d\n",i1 + 1,i2 + 1,i3 + 1,i4 + 1);
					for(int i = 0;i < index;i ++){
						if(idlehost[i] == 1){
							busy = 0;
							write(fdin[i][1],givehost,strlen(givehost));
							idlehost[i] = 0;
							break;
						}
					}
					if(busy){
						while(1){
							readfd = master;
							num = select(maxfd,&readfd,NULL,NULL,&timout);
							if(num > 0)	
								break;
						}
						for(int k = 2;k < maxfd;k ++){
							if(FD_ISSET(k,&readfd)){
								for(int i = 0;i < index;i ++){
									if(k == fdout[i][0]){
										memset(buf,0,sizeof(buf));
										read(fdout[i][0],buf,sizeof(buf));
										//printf("%s\n",buf);
										sscanf(buf,"%d %d\n%d %d\n%d %d\n%d %d\n",&playid[0],&prank[0],&playid[1],&prank[1],&playid[2],&prank[2],&playid[3],&prank[3]);
										for(int j = 0;j < 4;j ++){
											pscore[playid[j]] += (4 - prank[j]);
										}
										idlehost[i] = 1;
										break;
									}
								}
							}
						}
						for(int i = 0;i < index;i ++){
							if(idlehost[i] == 1){
								write(fdin[i][1],givehost,strlen(givehost));
								idlehost[i] = 0;
								break;
							}
						}
					}
				}
			}
		}
	}
		//printf("before\n");
	while(1){
		readfd = master;
		num = select(maxfd,&readfd,NULL,NULL,&timout);
		if(num == index)
			break;
	}
	for(int k = 2;k < maxfd;k ++){
		if(FD_ISSET(k,&readfd)){
			for(int i = 0;i < index;i ++){
				if(k == fdout[i][0]){
					memset(buf,0,sizeof(buf));
					read(fdout[i][0],buf,sizeof(buf));
					//printf("%s\n",buf);
					sscanf(buf,"%d %d\n%d %d\n%d %d\n%d %d\n",&playid[0],&prank[0],&playid[1],&prank[1],&playid[2],&prank[2],&playid[3],&prank[3]);
					for(int j = 0;j < 4;j ++){
						pscore[playid[j]] += (4 - prank[j]);
					}
					break;
				}
			}
		}
	}
	for(int i = 0;i < index;i ++){
		char stop[30] = "-1 -1 -1 -1\n";
		write(fdin[i][1],stop,strlen(stop));
	}
	int pfinal[22],max,premax = -1,maxpos,premaxpos;
	for(int i = 0;i < playnum;i ++){
		max = -1;
		for(int j = 0;j < playnum;j ++){
			if(pscore[j + 1] > max){
				max = pscore[j + 1];
				maxpos = j + 1;
			}
		}
		if(max == premax){
			pfinal[maxpos] = pfinal[premaxpos];
		}
		else{
			pfinal[maxpos] = i + 1;
			premaxpos = maxpos;
			premax = max;
		}
		pscore[maxpos] = -1;
	}
	for(int i = 0;i < playnum;i ++){
		memset(buf,0,sizeof(buf));
		sprintf(buf,"%d %d\n",i + 1,pfinal[i + 1]);
		write(STDOUT_FILENO,buf,strlen(buf));
	}
	for(int i = 0;i < index;i ++){
		waitpid(pid[i],NULL,0);
		close(fdout[i][0]);
		close(fdin[i][1]);
	}
	return 0;
}