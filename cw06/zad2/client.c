#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


#define MSGQOBJ_NAME    "/myqueue123"
#define MAX_MSG_LEN     10000

void send_message(mqd_t msgq_id, char * name, pid_t childPID, mqd_t myq_id){
    char message[MAX_MSG_LEN];
    fgets(message, MAX_MSG_LEN, stdin);
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    if(strcmp(message,"exit\n") == 0){
        mq_send(msgq_id, name, strlen(name)+1, 3);
        printf("exiting!\n");
        mq_close(myq_id);
        kill(childPID, SIGKILL);
        exit(0);
    }else{
        char temp[256];
        strcpy(temp, name);
        strcat(temp, " ");
        char timeString[8];
        sprintf(timeString, "%02i:%02i:%02i", timeinfo -> tm_hour, timeinfo->tm_min, timeinfo ->tm_sec);
        strcat(temp, timeString);
        strcat(temp, " ");
        strcat(temp, message);
        mq_send(msgq_id, temp, strlen(temp)+1, 2);
    }
}
void read_message(mqd_t myq_id){

    char message[256];
    unsigned int sender;
    int msgsz = mq_receive(myq_id, message, MAX_MSG_LEN, &sender);
    if(msgsz == -1){
        perror("In mq receive of client");
        exit(1);
    }
    printf("%s", message);
}

int main(int argc, char *argv[]) {
    mqd_t msgq_id;
    mqd_t myq_id;
   
    msgq_id = mq_open(MSGQOBJ_NAME, O_RDWR);
    
    if (msgq_id == (mqd_t)-1) {
        perror("In mq_open1()");
        exit(1);
    }
    char file[10];
    strcpy(file, "/");
    strcat(file, argv[1]);
    myq_id = mq_open(file, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, NULL);
    if (myq_id == (mqd_t)-1) {
        myq_id = mq_open(file, O_RDWR);
        if(myq_id == (mqd_t)-1){
            perror("In mq_open2()");
            exit(1); 
        }
    }
    mq_send(msgq_id, file, strlen(file)+1, 1);

    pid_t childPID;

        if((childPID = fork()) == -1){
            printf("Error forking in fork!\n");
        }
        else if(childPID == 0){
            while(1){
                read_message(myq_id);
            }
        }else{
            while(1){
                send_message(msgq_id, argv[1], childPID, myq_id);
            }
        }
    
    mq_close(msgq_id);
        
    return 0;
}
