#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
	pid_t firstpid;
	pid_t secondpid;
	int fd[2];

	if ((pipe(fd) == -1)) {
		printf("Failed to setup pipe\n");
	}
	if(((firstpid = fork()) == -1)){
		printf("Failed to fork\n");
	}

	if (firstpid == 0) {
		if (dup2(fd[1], STDOUT_FILENO) == -1){
			printf("Failed to redirect stdout of child\n");
		}
		else if ((close(fd[0]) == -1))
			printf("Failed to close extra pipe descriptors on ls\n");
		else {
			execlp("ls", "ls", "-l", NULL);
		}
	}
	int sd[2];
	if((pipe(sd) == -1)){
		printf("Failed to setup second pipe\n");
	}
	if(((secondpid = fork()) == -1)){
		printf("Failed second fork\n");
	}
	if( (secondpid == 0)){
		if (dup2(fd[0], STDIN_FILENO) == -1){
			printf("Failed to redirect stdin of middle\n");
		}
		if ((close(fd[1]) == -1))
			printf("Failed to close extra pipe file descriptors on middle\n");
		if (dup2(sd[1], STDOUT_FILENO)==-1)
			printf("Failed to redirect stdout of middle\n");
		if((close(sd[0]) == -1))
			printf("Failed to close extra socket on middle\n");
		execlp("grep", "grep", "^d", NULL);
		
	}else{
		if (dup2(sd[0], STDIN_FILENO) == -1){
			printf("Failed to redirect stdin of parent\n");
		}
		else if ((close(fd[1]) == -1)){
			printf("Failed to close extra pipe file descriptors on parent\n");
		}
		if(close(fd[0]) == -1){
			printf("Failed closing fd0 on parent\n");
		}
		if(close(sd[1])== -1){
			printf("Failed to close sd1 on parent\n");
		}
		else {
			execlp("wc", "wc", "-l", NULL);
		}
	}
	return 0;
} 