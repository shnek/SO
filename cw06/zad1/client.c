#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sched.h>
#include <sys/utsname.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <time.h>
#include <errno.h>

#define MAX_SEND_SIZE 80

struct mymsgbuf {
        long mtype;
        char mtext[MAX_SEND_SIZE];
};

void acquireConnection(int qid, struct mymsgbuf *qbuf, long type, char *text);
void read_message(int qid, struct mymsgbuf *qbuf, long type);
void send_message(int qid, struct mymsgbuf *qbuf, long type, char* name, pid_t childPID);

FILE *file;
int main(int argc, char *argv[])
{       
        file = fopen(argv[1], "w");
        key_t serverkey, mykey;
        int   msgqueue_id, myqueue_id;
        struct mymsgbuf qbuf;


        if((serverkey = ftok("server.c", 's'))==-1){
            printf("errno1: %s\n", strerror(errno));
            printf("Error ftoking\n");
            exit(1);
        }
        if((msgqueue_id = msgget(serverkey, IPC_CREAT|0660)) == -1) {
                perror("msgget");
                exit(1);
        }
        if((mykey = ftok(argv[1], 'c'))==-1){
            printf("errno2: %s\n", strerror(errno));
            printf("Error ftoking\n");
            exit(1);
        }

        if((myqueue_id = msgget(mykey, IPC_CREAT|0660)) == -1){
                perror("msgget on myself");
                exit(1);
        }
        printf("my queue id is: %i, key: %f\n", myqueue_id, (float) mykey);
        acquireConnection(msgqueue_id, (struct mymsgbuf *)&qbuf, 1,argv[1]); 

        pid_t childPID;

        if((childPID = fork()) == -1){
            printf("Error forking in fork!\n");
        }
        else if(childPID == 0){
            while(1){
                read_message(myqueue_id, &qbuf, 0);
            }
        }else{
            while(1){
                send_message(msgqueue_id, &qbuf, 2, argv[1], childPID);
            }
        }
        
        return(0);
}

void send_message(int qid, struct mymsgbuf *qbuf, long type, char* name, pid_t childPID){
    fgets(qbuf->mtext, 256, stdin);
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    if(strcmp(qbuf->mtext,"exit\n") == 0){
        printf("exiting!\n");
        qbuf -> mtype = 3;
        strcpy(qbuf->mtext, name);
        sprintf(qbuf->mtext, "%i", (int) ftok(name, 'c'));
        if((msgsnd(qid, (struct msgbuf *)qbuf,
                    strlen(qbuf->mtext)+1, 0)) ==-1)
            {
                    perror("msgsnd");
                    exit(1);
            }
            kill(childPID, SIGKILL);
            remove(name);
            fclose(file);
            exit(0);
    } else{
        char temp[256];
        strcpy(temp, qbuf->mtext);
        strcpy(qbuf->mtext, name);
        strcat(qbuf->mtext, " ");
        char timeString[8];
        sprintf(timeString, "%02i:%02i:%02i", timeinfo -> tm_hour, timeinfo->tm_min, timeinfo ->tm_sec);
        strcat(qbuf->mtext, timeString);
        strcat(qbuf->mtext, " ");
        strcat(qbuf->mtext, temp);
        qbuf -> mtype = type;
        if((msgsnd(qid, (struct msgbuf *)qbuf,
                    strlen(qbuf->mtext)+1, 0)) ==-1)
            {
                    perror("msgsnd");
                    exit(1);
            }
    }
    
}

void acquireConnection(int qid, struct mymsgbuf *qbuf, long type, char *text)
{
        qbuf->mtype = type;
        strcpy(qbuf->mtext, text);

        if((msgsnd(qid, (struct msgbuf *)qbuf,
                strlen(qbuf->mtext)+1, 0)) ==-1)
        {
                perror("msgsnd");
                exit(1);
        }
}

void read_message(int qid, struct mymsgbuf *qbuf, long type)
{
        qbuf->mtype = type;
        msgrcv(qid, (struct msgbuf *)qbuf, MAX_SEND_SIZE, type, 0);
        printf("%s", qbuf->mtext);
}