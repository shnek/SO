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

  FILE * theFile;
    theFile = fopen (filePath, "r+");

    int i;
    int j;

  struct tms start;
  struct tms end;
  fseek(theFile, 0, SEEK_END);
  amount = (int)ftell(theFile)/size;
  fseek(theFile, 0, SEEK_SET);
  times(&start);
  clock_t startTime = clock();
    for (i = 0; i < amount -1; ++i)
    {
      for (j = 0; j < amount - i - 1; j++)
      {
        fseek(theFile, j*size, SEEK_SET);
        fread(bufone, sizeof(char), size, theFile);
          fseek(theFile, (j+1)*size, SEEK_SET);
          fread(buftwo, sizeof(char), size, theFile);
          // printf("%s or %s? answer: %i\n", bufone, buftwo, compare(size, bufone, buftwo));
          if(compare(size, bufone, buftwo) == 1){
            fseek(theFile, j*size, SEEK_SET);
            fwrite(buftwo, sizeof(char), size, theFile);
            fseek(theFile, (j+1)*size, SEEK_SET);
            fwrite(bufone, sizeof(char), size, theFile);
          }
      }
    }
  printf("real: \t%f\n", ((double)(clock() - startTime)/(double)(CLOCKS_PER_SEC)));
  times(&end);
  printf("user: \t%f\n", (((double)(end.tms_utime)-(double)(start.tms_utime)) / (double)(sysconf(_SC_CLK_TCK))));
  printf("sys: \t%f\n\n", (((double)(end.tms_stime)-(double)(start.tms_stime)) / (double)(sysconf(_SC_CLK_TCK))));

    fclose(theFile);

  return 0;
}