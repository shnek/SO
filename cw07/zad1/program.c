#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

#define MAX_RETRIES 10
#define BUFFER_SIZE 10

union semun {
	int val;
	struct semid_ds *buf;
	ushort *array;
};

int initsem(key_t key, int nsems)
{
	int i;
	union semun arg;
	struct semid_ds buf;
	struct sembuf sb;
	int semid;

	semid = semget(key, nsems, IPC_CREAT | IPC_EXCL | 0666);

	if (semid >= 0) {
		sb.sem_op = 1; sb.sem_flg = 0;
		arg.val = 1;


		for(sb.sem_num = 0; sb.sem_num < nsems; sb.sem_num++) { 
			
			if (semop(semid, &sb, 1) == -1) {
				int e = errno;
				semctl(semid, 0, IPC_RMID);
				errno = e;
				return -1;
			}
		}

	} else if (errno == EEXIST) {
		int ready = 0;

		semid = semget(key, nsems, 0);
		if (semid < 0) return semid;

		arg.buf = &buf;
		for(i = 0; i < MAX_RETRIES && !ready; i++) {
			semctl(semid, nsems-1, IPC_STAT, arg);
			if (arg.buf->sem_otime != 0) {
				ready = 1;
			} else {
				sleep(1);
			}
		}
		if (!ready) {
			errno = ETIME;
			return -1;
		}
	} else {
		return semid;
	}

	return semid;
}

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


int main(int argc, char* argv[])
{
	key_t key;
	key_t mkey;
	int semid;
	struct sembuf sb;
	
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = SEM_UNDO;

	int shmid;
	int *data;

	
  	struct timeval tv;
  	time_t curtime;



  

	mkey = ftok("/my/file", 'R');
	shmid = shmget(mkey, 1024, 0644 | IPC_CREAT);

	if ((key = ftok("program.c", 'J')) == -1) {
		perror("ftok");
		exit(1);
	}

	if ((semid = initsem(key, 2)) == -1) {
		perror("initsem");
		exit(1);
	}
	if(argc > 1 && (strcmp(argv[1], "p") == 0)){
		srand(time(NULL));
		while(1){
			if (semop(semid, &sb, 1) == -1) {
				perror("semop");
				exit(1);
			}
			data = shmat(shmid, (void *)0, 0);
			int first = data[0];
			int length = data[1];
			if(length != BUFFER_SIZE){
				int temp = rand()%100000;
				data[(first+length)%BUFFER_SIZE + 2] = temp; 
				gettimeofday(&tv, NULL); 
				curtime=tv.tv_sec;
				char tbuffer[30];
 				strftime(tbuffer,30,"%T.",localtime(&curtime));
				printf("(%i, %s%ld) Dodalem liczbe: %i. Pozostalo zadan oczekujacych: %i.\n",(int)getpid(),tbuffer,tv.tv_usec, temp, data[1]);
				length++;
				data[1] = length;
			}
			sb.sem_op = 1; 
			if (semop(semid, &sb, 1) == -1) {
				perror("semop");
				exit(1);
			}
			usleep(100000 + rand()%10000);
		}
	}else if(argc > 1 && (strcmp(argv[1], "c") == 0)){
		srand(time(NULL));
		while(1){
			if (semop(semid, &sb, 1) == -1) {
				perror("semop");
				exit(1);
			}
			data = shmat(shmid, (void *)0, 0);
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
			sb.sem_op = 1;
			if (semop(semid, &sb, 1) == -1) {
				perror("semop");
				exit(1);
			}
			usleep(100000 + rand()%10000);
		}
	}else if(argc > 1 && (strcmp(argv[1], "r") == 0)){
			if (semop(semid, &sb, 1) == -1) {
				perror("semop");
				exit(1);
			}
			data = shmat(shmid, (void *)0, 0);
			int i;
			for(i = 0; i < BUFFER_SIZE+2; i++){
				data[i] = 0;
			}
			sb.sem_op = 1;
			if (semop(semid, &sb, 1) == -1) {
				perror("semop");
				exit(1);
			}
		}else if(argc > 1 && (strcmp(argv[1], "s") == 0)){
			if (semop(semid, &sb, 1) == -1) {
				perror("semop");
				exit(1);
			}
			data = shmat(shmid, (void *)0, 0);
			int i = 0;
			for(i = 0; i < BUFFER_SIZE+2; i++){
				printf("Tab[%i] = %i\n", i, data[i]);
			}
			sb.sem_op = 1;
			if (semop(semid, &sb, 1) == -1) {
				perror("semop");
				exit(1);
			}
		}else{
		printf("Aby uruchomic producenta nalezy wpisac ./program p\n");
		printf("Aby uruchomic konsumenta nalezy wpisac ./program c\n");
	}
	
	return 0;
}

	

