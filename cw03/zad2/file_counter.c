#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

const int czas = 15;

int main(int argc, char* argv[]){
	bool w = false;
	bool v = false;
	char *path;
	if(argc == 2){
		path = argv[1];
	}
	if(argc == 2 && argv[1][0] == '-'){
		if(argv[1][1] == 'w' || argv[1][2] == 'w') w = true;
		if(argv[1][1] == 'v' || argv[1][2] == 'v') v = true;
		path = ".";
	}
	if(argc == 3 && argv[2][0] == '-'){
		if(argv[2][1] == 'w' || argv[2][2] == 'w') w = true;
		if(argv[2][1] == 'v' || argv[2][2] == 'v') v = true;
		path = argv[1];
	}else{
		path = ".";
	}

	DIR *dp;
    struct dirent *dir;
    struct stat temp;
    dp =  opendir(path);
    pid_t childPID;
    if(dp == NULL){
       printf("error opeining directory\n");
        return -1;
    }
    int fileCounter = 0;
    int curFileCounter = 0;
    if(dp){
        while ((dir = readdir(dp)) != NULL){
            char test[80];
            strcpy(test, path);
            strcat(test, "/");
            strcat(test, dir->d_name);
            lstat(test, &temp);
            if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")){
                if(S_ISDIR(temp.st_mode)){
            	// if(version >=2) printf("Directory :: %s\n", dir->d_name);
                    char str[80];
                    strcpy(str, path);
                    // strcat(str, "/");
                    strcat(str, dir->d_name);
                    strcat(str, "/");
					if((childPID = fork()) < 0){
						printf("Error forking!\n");
					}
					else if(childPID == 0){
						// if(version >=2) printf("executing program on %s\n", str);
						if(v == false && w == false) {
							execl("./file_counter","", str, NULL);
						}else if (v == true && w == false) {
							execl("./file_counter","", str, "-v", NULL);
						}else if (v == false && w == true){
						 	execl("./file_counter","", str, "-w", NULL);
						}else{
							execl("./file_counter","", str,"-vw", NULL);
						}
						exit(fileCounter);
					}
                }else{
                    // if(version >=2) printf("File :: %s\n", dir->d_name);
                    curFileCounter++;
                    fileCounter++;
                }
            }
            
        }
        closedir(dp);
    }

    while (true) {
    int status;
    pid_t done = wait(&status);
    if (done == -1) {
        break; // no more child processes
    } else {
        if (!WIFEXITED(status) != 0) {
            printf("Child proces returned with an error!\n");
            exit(1);
        }else{
        	fileCounter += WEXITSTATUS(status);
        }
    }
}
    if(w == true) sleep(czas);
    if(v == true) printf("Total number exiting %s is %i\n", path, fileCounter);
    if(v == true) printf("Number of files in %s directory: %i\n", path, curFileCounter);
	exit(fileCounter);    
	return 0;
}
