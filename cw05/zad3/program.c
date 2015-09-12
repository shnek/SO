#include <stdio.h>
#include <unistd.h>
// #include <string.h>
#define MAX 256

int main(){
	char buffor[MAX];
	FILE *first = popen("ls -l", "r");
	if(first == NULL){
		printf("Error in first!\n");
		return 1;
	}
	int first_fdesc = fileno(first);
	if(dup2(first_fdesc, STDIN_FILENO) == -1){
		printf("Error redirecting first to stdin\n");
	}
	close(first_fdesc);
	FILE *second = popen("grep ^d", "r");
	if(second == NULL){
		printf("Error in second\n");
		return 1;
	}
	FILE *output = fopen("folders.txt", "w");
	while(fgets(buffor, MAX, second) != NULL){
		fprintf(output, "%s", buffor);
	}
	if(pclose(first) == -1){
		printf("Error closing first\n");
		return 1;
	}
	fclose(output);
	return 0; 
} 