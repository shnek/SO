#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAX_SEND_SIZE 80

struct mymsgbuf {
        long mtype;
        char mtext[MAX_SEND_SIZE];
};


void listenForClients(int qid, struct mymsgbuf *qbuf, long type);
int clientqueues[256];
int clientIterator = 0;



int main(int argc, char *argv[])
{
        int i;
        for(i = 0; i < 256; i++) clientqueues[i] = 0;
        key_t serverkey;
        int   msgqueue_id;
        struct mymsgbuf qbuf;

        serverkey = ftok("server.c", 's');


        if((msgqueue_id = msgget(serverkey, IPC_CREAT|0660)) == -1) {
                perror("msgget");
                exit(1);
        }
        
        while(1){
            listenForClients(msgqueue_id, &qbuf, 0);
        }
        
        return(0);
}

void clientConnectedAlert(char *text)
{
        struct mymsgbuf qbuf;
        qbuf.mtype = 2;
        strcpy(qbuf.mtext, "Client ");
        strcat(qbuf.mtext, text);
        strcat(qbuf.mtext, " connected!\0");
        int i;
        for(i = 0; i < clientIterator; i++){
            if((clientqueues[i]!=-1)&&(msgsnd(clientqueues[i], &qbuf,
                    strlen(qbuf.mtext)+1, 0)) ==-1)
            {
                printf("error!\n");
                    perror("msgsnd");
                    exit(1);
            }
        }
}

void sendMessageToClients(char *text){
        struct mymsgbuf qbuf;
        qbuf.mtype = 2;
        strcpy(qbuf.mtext, text);
        int i;
        for(i = 0; i < clientIterator; i++){
            if(clientqueues[i] != -1){
                if((msgsnd(clientqueues[i], &qbuf, strlen(qbuf.mtext)+1, 0)) ==-1){
                    printf("msgsnd error");
                    exit(1);
                } 
                printf("Sending to %i\n", clientqueues[i]);
            }
        }
}
void printClients(){
    int i;
    for(i = 0; i < clientIterator; i++){
        printf("Client nr %i\n", clientqueues[i]);
    }
}

void listenForClients(int qid, struct mymsgbuf *qbuf, long type)
{
        qbuf->mtype = type;
        msgrcv(qid, (struct msgbuf *)qbuf, MAX_SEND_SIZE, type, 0);
        if(qbuf->mtype == 1){
            printf("Client %s connected!\n", qbuf->mtext);
            key_t temp = ftok(qbuf->mtext, 'c');
            if((clientqueues[clientIterator] = msgget(temp, IPC_CREAT|0660)) == -1) {
                perror("msgget");
                exit(1);
            }
            clientIterator++;
            clientConnectedAlert(qbuf->mtext);
        } else if(qbuf->mtype == 2){
            printf("Message recieved: %s\n", qbuf->mtext);
            sendMessageToClients(qbuf->mtext);
        } else if(qbuf->mtype == 3){
            printf("Client %s disconnected\n", qbuf->mtext);
            int temp = atoi(qbuf->mtext);
            int i;
            for(i = 0; i < clientIterator; i++){
                if(clientqueues[i] == msgget(temp, IPC_CREAT|0660)){
                    clientqueues[i] = -1;
                }
            }
        }
}
