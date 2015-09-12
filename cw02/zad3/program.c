#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
	

const char* option_strings[] = {"setr", "setw", "list", "free", "read", "write"};

int checkCommand(char *option){
	int i;
		for (i = 0; i < 6; ++i)	{
			if (strcmp(option, option_strings[i]) == 0) {
				return i;		
			}
		}
	return -1;
}

int setr(char* path, struct flock* fLock, int pos) {
	int fd = open(path, O_RDONLY);

	if(fd < 0) {
		printf("Error opening file!\n");
		return -1;
	}
	fLock->l_type = F_RDLCK;
	fLock->l_start = pos;
	fLock->l_whence = SEEK_SET;
	fLock->l_len = 1;
	if (fcntl(fd, F_SETLK, fLock) < 0) {
		printf("Error setting read lock!\n");
		return -1;
	}
	return 0;
}

int setw(char* path, struct flock* fLock, int pos) {
	int fd = open(path, O_WRONLY);

	if(fd < 0) {
		printf("Error opening file!\n");
		return -1;
	}

	fLock->l_type = F_WRLCK;
	fLock->l_start = pos;
	fLock->l_whence = SEEK_SET;
	fLock->l_len = 1;
	if (fcntl(fd, F_SETLK, fLock) < 0) {
		printf("Error setting write lock!\n");
		return -1;
	}
	return 0;
}

int freeL(char* path, struct flock* fLock, int pos) {
	int fd = open(path, O_RDWR);

	if(fd < 0) {
		printf("Error opening file!\n");
		return -1;
	}
	fLock->l_type = F_UNLCK;
	fLock->l_start = pos;
	fLock->l_whence = SEEK_SET;
	fLock->l_len = 1;
	if ((fcntl(fd, F_SETLK, fLock)) != 0) {
		printf("Error freeing lock!\n");
		return -1;
	}
	return 0;
}

int readChar(char *path, int pos){
	struct flock fLock;
	fLock.l_start = pos;
	fLock.l_type = F_RDLCK;
	fLock.l_whence = SEEK_SET;
	fLock.l_len = 1;
	int fd = open(path, O_RDONLY);

	if(fd < 0) {
		printf("Error opening file!\n");
		return -1;
	}
	char *ourChar = malloc(sizeof(char));
	lseek(fd, pos, SEEK_SET);
	if(fcntl(fd, F_GETLK, fLock) == 0){
		printf("fcntl error!\n");
		return;
	}
	if(fLock.l_type != F_RDLCK){
		if ((read(fd, ourChar, 1)) < 0) {
			printf("Error in readChar!\n");
			return -1;
		}
		printf("Szukany znak to %c \n", *ourChar);
	}else printf("Char read is locked!\n");
	free(ourChar);
	return 0;
}

int writeChar(char *path, int pos, char *ourChar){
	struct flock fLock;
	fLock.l_start = pos;
	fLock.l_type = F_RDLCK;
	fLock.l_whence = SEEK_SET;
	fLock.l_len = 1;
	int fd = open(path, O_WRONLY);

	if(fd < 0) {
		printf("Error opening file!\n");
		return -1;
	}
	lseek(fd, pos, SEEK_SET);
	if(fcntl(fd, F_GETLK, &fLock) != 0){
		printf("fcntl error!\n");
		return;
	}
	if(fLock.l_type != F_WRLCK){
		if ((write(fd, ourChar, 1)) < 0) {
			printf("Error in writeChar!\n");
			return -1;
		}
	}else printf("Char write is locked!\n");
	return 0;
}


int list(char* path, struct flock* fLock) {
	char* character = malloc(sizeof(char));
	int fd = open(path,O_RDWR);

	if(fd < 0){
		printf("Error opening file in list\n");
		return 1;
	}
	fLock->l_type = F_WRLCK;
	fLock->l_whence = SEEK_CUR;
	fLock->l_start = -1;
	fLock->l_len = 1;
	
	int i;
	for(i = 0; read(fd, character, 1) > 0; ++i) {
		fLock->l_type = F_WRLCK;
		fLock->l_whence = SEEK_CUR;
		fLock->l_start = -1;		
		
		if (fcntl(fd, F_GETLK, fLock)) {
			printf("Error reading lock!\n");
			return 1;
		}
		if (fLock->l_type == F_WRLCK) printf("%d\t zapis, PID: %d\n", i, fLock->l_pid);
		if (fLock->l_type == F_RDLCK) printf("%d\t odczyt, PID: %d\n", i, fLock->l_pid);
	}
	free(character);
	return 0;
}




void ask(char *path){

	char *command = malloc(sizeof(char)*5);
	if (command == NULL) {
		printf("Command malloc error!\n");
	}

	struct flock* fLock = malloc(sizeof(struct flock));
	if (fLock == NULL) {
		printf("Flock malloc error!\n");
	}



	printf("Wpisz polecenie: setr/setw/list/free/read/write\n");
	if (scanf("%s", command) < 0) {
		printf("Scanf error!\n");
	}
	while ( !(checkCommand(command) >= 0 && checkCommand(command) < 6 ) ) {
		printf("Zle wpisane polecenie, wpisz ponownie: \n");
		if (scanf("%s", command) < 0) {
			printf("Scanf error!\n");
		}
	}
	struct stat temp;
	lstat(path, &temp);
	if(checkCommand(command) == 2){
		list(path, fLock);
		return;
	}

	printf("Ktory znak?\n");
	char *number = malloc(sizeof(char)*5);
	if(number == NULL){
		printf("Malloc number error!\n");
	}
	if(scanf("%s", number) < 0) {
		printf("Scanf error!\n");
	}
	int theNumber = atoi(number);

	if(checkCommand(command) == 0)
		if(setr(path, fLock, theNumber)==0);
	
	if(checkCommand(command) == 1) 
		if(setw(path, fLock, theNumber)==0);

	if(checkCommand(command) == 3) freeL(path, fLock, theNumber);

	if(checkCommand(command) == 4) readChar(path, theNumber);

	if(checkCommand(command) == 5) {
		printf("Co za znak wpisac?\n");
		char *ourChar = malloc(sizeof(char)*5);
		if(ourChar == NULL){
			printf("Malloc char error!\n");
			return;
		}
		if(scanf("%s", ourChar) < 0) {
			printf("Scanf error!\n");
			return;
		}
		writeChar(path, theNumber, ourChar);
	}
	ask(path);

	free(number);
	free(command);
	free(fLock);
	return;
}


int main(int argc, char* argv[]){

    char* path = argv[1];

    ask(path);
    

  return 0;
}
