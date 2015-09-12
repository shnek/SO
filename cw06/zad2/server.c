#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


#define MSGQOBJ_NAME    "/myqueue123"
#define MAX_MSG_LEN     10000

 char clientqueues[50][256];
 int clientIterator = 0;

void removeFromClients(char *name){
    printf("%s disconnected\n", name);
    char temp[80];
    strcpy(temp, "/");
    strcat(temp, name);
    int i;
    for(i = 0; i < clientIterator; i++){
        if(strcmp(clientqueues[i], temp) == 0 ){
            strcpy(clientqueues[i], "");
        }
    }
}

void sendMessage(char *message){
    int i;
    mqd_t destq_id;
    for(i = 0; i < clientIterator; i++){
        if(strcmp(clientqueues[i], "") !=0){
            destq_id = mq_open(clientqueues[i], O_RDWR);
            mq_send(destq_id, message, strlen(message)+1, 1);
        }
    }
}

int main(int argc, char *argv[]) {
    mqd_t msgq_id;
    char msgcontent[MAX_MSG_LEN];
    int msgsz;
    unsigned int sender;
    struct mq_attr msgq_attr;
    
    msgq_id = mq_open(MSGQOBJ_NAME, O_RDWR);
    if (msgq_id == (mqd_t)-1) {
        msgq_id = mq_open(MSGQOBJ_NAME,O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, NULL);
        if(msgq_id == (mqd_t)-1){
            perror("In mq_open()");
            exit(1);
        }
    }

    mq_getattr(msgq_id, &msgq_attr);

    while(1){
        msgsz = mq_receive(msgq_id, msgcontent, MAX_MSG_LEN, &sender);
        if (msgsz == -1) {
            perror("In mq_receive()");
            exit(1);
        }
        switch(sender){
            case 1:
                printf("Client %s connected!\n", msgcontent);
                strcpy(clientqueues[clientIterator], msgcontent);
                clientIterator++;
                break;
            case 2:
                printf("%s", msgcontent);
                sendMessage(msgcontent);
                break;
            case 3:
                removeFromClients(msgcontent);
                break;
        }
    }
    mq_close(msgq_id);
    return 0;
}
