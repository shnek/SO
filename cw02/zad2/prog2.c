#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <ftw.h>

char permitions[9];

int list(const char *name, const struct stat *status, int type) {
 if(type == FTW_NS)
  return 0;

 if(type == FTW_F){
    time_t mytime;
    mytime = status -> st_mtime;

    int ifprint = 1;
    if(((permitions[0] == 'r')&&!(status ->st_mode & S_IRUSR))) ifprint = 0;
    if(((permitions[1] == 'w')&&!(status ->st_mode & S_IWUSR))) ifprint = 0;
    if(((permitions[2] == 'x')&&!(status ->st_mode & S_IXUSR))) ifprint = 0;
    if(((permitions[3] == 'r')&&!(status ->st_mode & S_IRGRP))) ifprint = 0;
    if(((permitions[4] == 'w')&&!(status ->st_mode & S_IWGRP))) ifprint = 0;
    if(((permitions[5] == 'x')&&!(status ->st_mode & S_IXGRP))) ifprint = 0;
    if(((permitions[6] == 'r')&&!(status ->st_mode & S_IROTH))) ifprint = 0;
    if(((permitions[7] == 'w')&&!(status ->st_mode & S_IWOTH))) ifprint = 0;
    if(((permitions[8] == 'x')&&!(status ->st_mode & S_IXOTH))) ifprint = 0;

    if(ifprint == 1){
        printf("Filename: %s, Permissions: ", name);
        printf( (S_ISDIR(status ->st_mode)) ? "d" : "-");
        printf( (status ->st_mode & S_IRUSR) ? "r" : "-");
        printf( (status ->st_mode & S_IWUSR) ? "w" : "-");
        printf( (status ->st_mode & S_IXUSR) ? "x" : "-");
        printf( (status ->st_mode & S_IRGRP) ? "r" : "-");
        printf( (status ->st_mode & S_IWGRP) ? "w" : "-");
        printf( (status ->st_mode & S_IXGRP) ? "x" : "-");
        printf( (status ->st_mode & S_IROTH) ? "r" : "-");
        printf( (status ->st_mode & S_IWOTH) ? "w" : "-");
        printf( (status ->st_mode & S_IXOTH) ? "x" : "-");
        printf(", Size: %i, Last Mod: %s", status ->st_size, ctime(&mytime));    
    }
    // printf("Filename: %s, Permissions: ", name);
    // printf( (S_ISDIR(status -> st_mode)) ? "d" : "-");
    // printf( (status -> st_mode & S_IRUSR) ? "r" : "-");
    // printf( (status -> st_mode & S_IWUSR) ? "w" : "-");
    // printf( (status -> st_mode & S_IXUSR) ? "x" : "-");
    // printf( (status -> st_mode & S_IRGRP) ? "r" : "-");
    // printf( (status -> st_mode & S_IWGRP) ? "w" : "-");
    // printf( (status -> st_mode & S_IXGRP) ? "x" : "-");
    // printf( (status -> st_mode & S_IROTH) ? "r" : "-");
    // printf( (status -> st_mode & S_IWOTH) ? "w" : "-");
    // printf( (status -> st_mode & S_IXOTH) ? "x" : "-");
    // printf(", Size: %i, Last Mod: %s", status -> st_size, ctime(&mytime)); 
 }
 return 0;
}

int main(int argc, char* argv[]){

    printf("\n");
    char* path = argv[1];
    char* permits = argv[2];

    int i;
    for(i = 0; i < 9; i++){
        permitions[i] = permits[i];
    }

    ftw(path, list, 1);
    

  return 0;
}
