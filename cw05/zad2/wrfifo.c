#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define BUF_SIZE 256



int main(int argc, char* argv[]){
	int fd;
	char* fifoFile;
	if(argc!= 2){
		printf("Arugment number error!\n");
		return 1;
	}else{
		fifoFile = argv[1];
	}
	char buffor[BUF_SIZE];
	printf("Type in message:\n");
	fgets(buffor, BUF_SIZE, stdin);
	if ((strlen(buffor)>0) && (buffor[strlen (buffor) - 1] == '\n'))
        buffor[strlen (buffor) - 1] = '\0';

	char temp[10];
	char output[BUF_SIZE];
	strcpy(output, temp);
	strcat(output, " - ");
	if((fd = open(fifoFile, O_WRONLY)) == -1){
		printf("error opening file\n");
	}
	time_t now;
	struct tm *now_tm;
	now = time(NULL);
	now_tm = localtime(&now);
	snprintf(output, 256, "%i - %i:%i:%i - %s", (int)getpid(), now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec, buffor);

	write(fd,output, (int)strlen(output)+1);
	close(fd);

	return 0;
}