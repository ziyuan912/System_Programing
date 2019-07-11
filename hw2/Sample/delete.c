#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/wait.h>
int main(int argc, char const *argv[]) {
	char read_buf[30], write_buf[4][30];
	sprintf(read_buf, "host%s.FIFO", argv[1]);
	unlink(read_buf);
	for (int i = 0; i < 4; i++){
		sprintf(write_buf[i], "host%s_%c.FIFO", argv[1], 'A'+i);
		unlink(write_buf[i]);
	}
	return 0;
}