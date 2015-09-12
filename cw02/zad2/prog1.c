#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

void reddir(char* direct, char* permits){
    DIR *dp;
    struct dirent *dir;
    struct stat temp;
    dp =  opendir(direct);
    if(dp == NULL){
       printf("error opeining directory\n");
        return;
    }
    if (dp){
        while ((dir = readdir(dp)) != NULL){
            char test[80];
            strcpy(test, direct);
            strcat(test, "/");
            strcat(test, dir->d_name);
            lstat(test, &temp);
            if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")){
                if(S_ISDIR(temp.st_mode)){
                    char str[80];
                    strcpy(str, direct);
                    strcat(str, "/");
                    strcat(str, dir->d_name);
                    reddir(str, permits);
                }else if((temp.st_mode & S_IFMT) == S_IFREG){
                    time_t mytime;
                    mytime = temp.st_mtime;
                    int ifprint = 1;
                    if(((permits[0] == 'r')&&!(temp.st_mode & S_IRUSR))) ifprint = 0;
                    if(((permits[1] == 'w')&&!(temp.st_mode & S_IWUSR))) ifprint = 0;
                    if(((permits[2] == 'x')&&!(temp.st_mode & S_IXUSR))) ifprint = 0;
                    if(((permits[3] == 'r')&&!(temp.st_mode & S_IRGRP))) ifprint = 0;
                    if(((permits[4] == 'w')&&!(temp.st_mode & S_IWGRP))) ifprint = 0;
                    if(((permits[5] == 'x')&&!(temp.st_mode & S_IXGRP))) ifprint = 0;
                    if(((permits[6] == 'r')&&!(temp.st_mode & S_IROTH))) ifprint = 0;
                    if(((permits[7] == 'w')&&!(temp.st_mode & S_IWOTH))) ifprint = 0;
                    if(((permits[8] == 'x')&&!(temp.st_mode & S_IXOTH))) ifprint = 0;

                    if(ifprint == 1){
                        printf("Filename: %s, Permissions: ", dir->d_name);
                        printf( (S_ISDIR(temp.st_mode)) ? "d" : "-");
                        printf( (temp.st_mode & S_IRUSR) ? "r" : "-");
                        printf( (temp.st_mode & S_IWUSR) ? "w" : "-");
                        printf( (temp.st_mode & S_IXUSR) ? "x" : "-");
                        printf( (temp.st_mode & S_IRGRP) ? "r" : "-");
                        printf( (temp.st_mode & S_IWGRP) ? "w" : "-");
                        printf( (temp.st_mode & S_IXGRP) ? "x" : "-");
                        printf( (temp.st_mode & S_IROTH) ? "r" : "-");
                        printf( (temp.st_mode & S_IWOTH) ? "w" : "-");
                        printf( (temp.st_mode & S_IXOTH) ? "x" : "-");
                        printf(", Size: %i, Last Mod: %s", temp.st_size, ctime(&mytime));    
                    }
                }
            }
            
        }
        closedir(dp);
    }
    return;
}

int main(int argc, char* argv[]){

    printf("\n");
    char* path = argv[1];
    char* permits = argv[2];

    reddir(path, permits);
    

  return 0;
}
