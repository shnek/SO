#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

void generateData(int size, char* array){
	int i;
	for(i = 0; i < size; i++){
		array[i] = (char)(rand()%24 + 97);
	}
	array[size+1] = (char)('\0');
	return;
}

int main(int argc, char* argv[]){

	char* filePath = argv[1];
	int size = atoi(argv[2]);
	int amount = atoi(argv[3]);
	srand(time(NULL));



    // printf("File path: %s\n", filePath);
	int myFile;
	myFile = open(filePath,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR|S_IWOTH|S_IROTH|S_IRGRP|S_IWGRP);
	if(myFile == -1) printf("Error creating file to write\n");

	char *buffor = malloc(size*(sizeof(char)+1));
	int error;

	int i;
    for (i = 0; i < amount; ++i)
    {
    	generateData(size, buffor);
      	// printf("%s\n", buffor);
    	error = write(myFile, buffor, size);
    	if(error == -1) printf("Error writting data to file\n");
    }


	if(close(myFile)==-1) printf("Error closing file\n");
    

	return 0;
}