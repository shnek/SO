#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/mman.h>


#define BUFFER_SIZE 10

int analyzeValue(int theValue){
	int i;
	if(theValue < 3) return 1;
	double end = sqrt(1.0*theValue);
	for(i = theValue-1; i > end; i--){
		if(theValue%i == 0){
			return 0;
		}
	}
	return 1;
}

sem_t *semaphore;

int main(int argc, char* argv[])
{
	
	int fd;
	
	struct timeval tv;
  	time_t curtime;

	int *data;
	fd = shm_open("program.c", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
		if (fd == -1){
			perror("shmopen error");
			exit(1);
		}


	if (ftruncate(fd, sizeof(int)) == -1){
		perror("Ftrunkate error");
		exit(1);
	}
	data = mmap((caddr_t)0, (BUFFER_SIZE+2)*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	semaphore = sem_open("pSem", O_CREAT, 0644, 1);


	if(argc > 1 && (strcmp(argv[1], "r") == 0)){
		sem_wait(semaphore);
		int i;
		for(i = 0; i < (BUFFER_SIZE+2); i++){
			data[i] = 0;
		}
		sem_post(semaphore);
	}else if(argc > 1 && (strcmp(argv[1], "s") == 0)){
		sem_wait(semaphore);
		int i; 
		for(i = 0; i < (BUFFER_SIZE+2); i++){
			printf("Tab[%i] = %i\n", i, data[i]);
		}
		sem_post(semaphore);
	}else if(argc > 1 && (strcmp(argv[1], "c") == 0)){
		srand(time(NULL));
		while(1){
			sem_wait(semaphore);
			int first = data[0];
			int length = data[1];
			int theValue;
			if(length != 0){
				theValue = data[first + 2];
				gettimeofday(&tv, NULL); 
				data[first + 2] = 0;
				if(first == BUFFER_SIZE-1){
					first = 0;
				}else{
					first = first + 1;
				}
				data[0] = first;
				data[1] = data[1] - 1;

  				curtime=tv.tv_sec;
				char tbuffer[30];
 				strftime(tbuffer,30,"%T.",localtime(&curtime));
  				
				if(analyzeValue(theValue) == 1){
					printf("(%i, %s%ld) Sprawdzilem liczbe %i - pierwsza. Pozostalo zadan oczekujacych: %i\n",(int)getpid(),tbuffer,tv.tv_usec, theValue, data[1]);
				}else{
					printf("(%i, %s%ld) Sprawdzilem liczbe %i - zlozona. Pozostalo zadan oczekujacych: %i\n",(int)getpid(),tbuffer,tv.tv_usec, theValue, data[1]);
				}
			}
			sem_post(semaphore);
			usleep(1000000+rand()%10000);
		}
	}else if(argc > 1 && (strcmp(argv[1], "p") == 0)){
		srand(time(NULL));
		while(1){
			sem_wait(semaphore);
			int first = data[0];
			int length = data[1];
			if(length != BUFFER_SIZE){
				int temp = rand()%100;
				data[(first+length)%BUFFER_SIZE + 2] = temp; 
				gettimeofday(&tv, NULL); 
				curtime=tv.tv_sec;
				char tbuffer[30];
 				strftime(tbuffer,30,"%T.",localtime(&curtime));
				printf("(%i, %s%ld) Dodalem liczbe: %i. Pozostalo zadan oczekujacych: %i.\n",(int)getpid(),tbuffer,tv.tv_usec, temp, data[1]);
				length++;
				data[1] = length;
			}
			sem_post(semaphore);
			usleep(1000000 + rand()%10000);
		}
	}else{
		printf("Aby uruchomic producenta nalezy wpisac ./program p\n");
		printf("Aby uruchomic konsumenta nalezy wpisac ./program c\n");
	}


	return 0;
}

	

