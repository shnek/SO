#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>

#define BUF_SIZE 256

char* fifoFile;

int main(int argc, char* argv[]){
	int fd;
	if(argc!= 2){
		fifoFile = "/tmp/fifoTest";
	}else{
		fifoFile = argv[1];
	}

	if(mkfifo(fifoFile,0666) == -1 && errno != EEXIST){
		printf("error setting fifo\n");
	}
	
	if((fd = open(fifoFile, O_RDWR)) == -1){
		printf("error opening file\n");
	}
	char buffor[BUF_SIZE];
	time_t now;
	struct tm *now_tm;
	while(true){
		if(read(fd,buffor,BUF_SIZE)!=0){
			now = time(NULL);
			now_tm = localtime(&now);
			printf("%i:%i:%i -  %s\n", now_tm->tm_hour, now_tm->tm_min, now_tm -> tm_sec, buffor);			
		}
	}
	close(fd);

	unlink(fifoFile);
	return 0;

}