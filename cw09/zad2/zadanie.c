#include <time.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define BUFFOR_LENGTH 100
sem_t wolne;
sem_t mutex;
int readers_number;
int writers_number;
pthread_t *readers;
pthread_t *writers;
char *text;
int buffor[BUFFOR_LENGTH];
unsigned int microseconds = 1000000;
int searchValue;


void writeSomething(){
    int i;
    int iterations = rand()%20;
    for(i = 0; i < iterations; i++){
        buffor[rand()%BUFFOR_LENGTH] = rand()%100;
    }
    printf("Writer modified the buffor!\n");
}
void readAll(int number){
    int counter = 0;
    int i;
    for(i = 0; i <  BUFFOR_LENGTH; i++){
        if(buffor[i] == number){
            counter++;
        }
    }
    printf("Found %i number %i many times\n", number, counter);
}
void* writerFunction(void *arg)
{
    pthread_t id = pthread_self();

    int idno;
    for(idno = 0; idno < writers_number; idno++){
        if(pthread_equal(id,writers[idno])){
            break;
        }
    }
    while(1){
        usleep(microseconds);
        sem_wait(&mutex);
        int i = 0;
        for(i = 0; i < readers_number; i++){
            sem_wait(&wolne);
        }
        writeSomething();
        for(i = 0; i < readers_number; i++){
            sem_post(&wolne);
        }
        sem_post(&mutex);
    }
    return NULL;
}
void* readerFunction(void *arg)
{
    pthread_t id = pthread_self();

    int idno;
    for(idno = 0; idno < readers_number; idno++){
        if(pthread_equal(id,readers[idno])){
            break;
        }
    }
    while(1){
        usleep(microseconds);
        sem_wait(&wolne);
        // printf("Got semaphore! %i\n", idno);
        readAll(searchValue);
        usleep(microseconds);
        // printf("Releasing semaphre! %i\n", idno);
        sem_post(&wolne);
    }
    return NULL;
}


int main(int argc, char* argv[])
{
    if(argc == 0){
        searchValue = 1;
    }else{
        searchValue = atoi(argv[1]);
    }
    srand(time(NULL));
    writers_number = 5;
    readers_number = 5;
    sem_init(&wolne, 0, readers_number);
    sem_init(&mutex, 0, 1);
    int i = 0;
    int err;
    readers = malloc(readers_number * sizeof(pthread_t));
    writers = malloc(writers_number * sizeof(pthread_t));

    printf("Creating %i readers\n", readers_number);
    while(i < readers_number)
    {
        err = pthread_create(&(readers[i]), NULL, &readerFunction, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        i++;
    }
    i = 0;
    printf("Creating %i writers\n", writers_number);
    while(i < writers_number)
    {
        err = pthread_create(&(writers[i]), NULL, &writerFunction, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        i++;
    }
    for(i = 0; i < readers_number; i++){
        pthread_join(readers[i], NULL);
    }
    for(i = 0; i < writers_number; i++){
        pthread_join(writers[i], NULL);
    }

    return 0;
}