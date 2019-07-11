/*bo5902050 黃子源*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>

typedef struct {
    char c_time_string[100];
} TimeInfo;

int main(int argc, char const *argv[])
{
	char filename[1024],buf[1024];
	int fd,i;
    time_t current_time;
    char c_time_string[100];
    /*TimeInfo *p_map;
    const char  *file ="time_test";

	fd = open(file, O_RDWR | O_TRUNC | O_CREAT, 0777); 
    if(fd<0){
        perror("open");
        exit(-1);
    }
    lseek(fd,sizeof(TimeInfo),SEEK_SET);
    write(fd,"",1);

    p_map = (TimeInfo*) mmap(0, sizeof(TimeInfo), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    printf("mmap address:%#x\n",(unsigned int)&p_map); // 0x00000
    close(fd);


    current_time = time(NULL);
    strcpy(c_time_string, ctime(&current_time));

    memcpy(p_map->c_time_string, &c_time_string , sizeof(c_time_string));
    
    printf("initialize over\n ");

    munmap(p_map, sizeof(TimeInfo));
    printf("umap ok \n");


    fd = open(file, O_RDWR);
    p_map = (TimeInfo*)mmap(0, sizeof(TimeInfo),  PROT_READ,  MAP_SHARED, fd, 0);
    printf("%s\n", p_map->c_time_string);*/
	read(0,filename,sizeof(filename));
	//printf("filereader_%s\n",filename );
	fd = open(filename,O_RDONLY);
	int readlen;
	if(fd < 0){
		write(1,"404 Not Found",13);
		exit(1);
	}
	while((readlen = read(fd,buf,sizeof(buf))) > 0){
		write(1,buf,readlen);
	}
    TimeInfo *p_map;
    const char  *file ="time_test";
    
    fd = open(file, O_RDWR | O_TRUNC | O_CREAT, 0777); 
    if(fd<0)
    {
        perror("open");
        exit(-1);
    }
    lseek(fd,sizeof(TimeInfo),SEEK_SET);
    write(fd,"",1);

    p_map = (TimeInfo*) mmap(0, sizeof(TimeInfo), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    //printf("mmap address:%#x\n",(unsigned int)&p_map); // 0x00000
    close(fd);


    current_time = time(NULL);
    strcpy(c_time_string, ctime(&current_time));

    memcpy(p_map->c_time_string, &c_time_string , sizeof(c_time_string));
    
    //printf("initialize over\n ");

    munmap(p_map, sizeof(TimeInfo));
	return 0;
}