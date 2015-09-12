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

	char *buffor = malloc(size*(sizeof(char)+1));
	srand(time(NULL));
	
	FILE * theFile;
    theFile = fopen (filePath, "w+");

    int i;
    for (i = 0; i < amount; ++i)
    {
    	generateData(size, buffor);
    	// printf("%s\n", buffor);
    	fwrite(buffor, sizeof(char), size, theFile);
    }

    fclose(theFile);




	return 0;
}