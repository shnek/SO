#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>

#define BUF_SIZE 1020
pthread_t *tid;
FILE *target;
char *text;
int thread_number;
int record_number;
int isFound = 0;

struct record{
    int id;
    char text[BUF_SIZE];
}record;

int containsString(char* buffer, char* text){
    if(strstr(buffer, text)!= NULL){
        return 1;
    }
    return 0;
}

void* doSomeThing(void *arg)
{
    pthread_t id = pthread_self();

    int idno;
    for(idno = 0; idno < thread_number; idno++){
        if(pthread_equal(id,tid[idno])){
            break;
        }
    }
    // printf("Thread %i processing\n", idno);
        int endOfFile = 0;
        while((endOfFile == 0)&&(isFound==0)){
            struct record theRecord[record_number];
            int i;
            if(fread(&theRecord, 1024*record_number, 1, target)==0){
                endOfFile = 1;
            }
            for(i = 0; i < record_number; i++){
                // if(fread(&theRecord[i], 1024, 1, target) == 0){
                //     endOfFile = 1;
                // } 
                if(containsString(theRecord[i].text, text)){
                    printf("Found! TID: %i, Record ID: %i\n", idno, theRecord[i].id);
                    isFound = 1;
                    break;
                }
            }

            
        }
    return NULL;
}


int main(int argc, char* argv[])
{
    if(argc < 4 || argc > 5){
        printf("Podano za mala liczbe argumentow\nProsze podac: ./zadanie <liczba watkow> <szukane slowo> <ilosc rekordow>\n");
        return 0;
    }
    text = malloc(sizeof(char)*100);
    strcpy(text, argv[2]);
    int i = 0;
    int err;
    thread_number = atoi(argv[1]);
    tid = malloc(thread_number * sizeof(pthread_t));
    printf("Creating %i threads\n", thread_number);
    target = fopen("pan-tadeusz.txt", "r");
    record_number = atoi(argv[3]);
    while(i < thread_number)
    {
        err = pthread_create(&(tid[i]), NULL, &doSomeThing, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        else
            // printf("\n Thread created successfully\n");
        i++;
    }
    for(i = 0; i < thread_number; i++){
        pthread_join(tid[i], NULL);
    }

    // sleep(2);
    return 0;
}