#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>

int compare(int size, char* first, char* second){
	int i;
	for(i = 0; i < size; i++){
		if((int)first[i] > (int)second[i]){
			return 1;
		}else if((int)first[i] < (int)second[i]){
			return 0;
		}
	}
	return 0;
}

int main(int argc, char* argv[]){

	char* filePath = argv[1];
	int size = atoi(argv[2]);
	int amount;

	char *bufone = malloc(size*(sizeof(char)+1));
	char *buftwo = malloc(size*(sizeof(char)+1));

	int myFile;
    myFile = open(filePath,O_CREAT|O_RDWR,S_IRUSR|S_IWUSR|S_IWOTH|S_IROTH|S_IRGRP|S_IWGRP);

    int i;
    int j;

  amount = (int)lseek(myFile, 0, SEEK_END);
  amount /= size;
  struct tms start;
  struct tms end;
  times(&start);
  clock_t startTime = clock();
    for (i = 0; i < amount; ++i)
    {
    	for (j = 0; j < amount; j++)
    	{
    		lseek(myFile, j*size, SEEK_SET);
    		read(myFile, bufone, size);
  		  	lseek(myFile, (j+1)*size, SEEK_SET);
  		  	read(myFile, buftwo, size);
  		  	// printf("%s or %s? answer: %i\n", bufone, buftwo, compare(size, bufone, buftwo));
  		  	if(compare(size, bufone, buftwo) == 1){
  		  		lseek(myFile, j*size, SEEK_SET);
  		  		write(myFile, buftwo, size);
  		  		lseek(myFile, (j+1)*size, SEEK_SET);
  		  		write(myFile, bufone, size);
  		  	}
    	}
    }
  printf("real: \t%f\n", ((double)(clock() - startTime)/(double)(CLOCKS_PER_SEC)));
  times(&end);
  printf("user: \t%f\n", (((double)(end.tms_utime)-(double)(start.tms_utime)) / (double)(sysconf(_SC_CLK_TCK))));
  printf("sys: \t%f\n\n", (((double)(end.tms_stime)-(double)(start.tms_stime)) / (double)(sysconf(_SC_CLK_TCK))));

    if(close(myFile)==-1) printf("Error closing file\n");

	return 0;
}