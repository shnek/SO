
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

pthread_t *tid;
int target;
char *text;
int thread_number;
int record_number;
pthread_mutex_t mutex;
int forks[5];
unsigned int microseconds = 1000;
pthread_cond_t conditions[5];

void getForkLeft(int idno){
    pthread_mutex_lock(&mutex);
    if(idno == 0) idno = 5;
    while(forks[idno-1] == 1){
        pthread_cond_wait(&conditions[idno-1], &mutex);
    }
    forks[idno-1] = 1;
    pthread_mutex_unlock(&mutex);
}
void getForkRight(int idno){
    pthread_mutex_lock(&mutex);
    while(forks[idno] == 1){
        pthread_cond_wait(&conditions[idno], &mutex);
    }
    forks[idno] = 1;
    pthread_mutex_unlock(&mutex);
}
void putForkLeft(int idno){
    pthread_mutex_lock(&mutex);
    if(idno == 0) idno = 5;
    forks[idno - 1] = 0;
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&conditions[idno-1]);
}
void putForkRight(int idno){
    pthread_mutex_lock(&mutex);
    forks[idno] = 0;
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&conditions[idno]);
}
void* doSomeThing(void *arg)
{
    pthread_t id = pthread_self();
    int timesEaten = 0;

    int idno;
    for(idno = 0; idno < thread_number; idno++){
        if(pthread_equal(id,tid[idno])){
            break;
        }
    }
    while(1){
        usleep(microseconds);
        if(idno == 0 || idno == 2){
            getForkLeft(idno);
            getForkRight(idno);
            timesEaten = timesEaten+1;
            printf("Philosopher %i is eating %i times!\n", idno, timesEaten);
            usleep(microseconds);
            putForkRight(idno);
            putForkLeft(idno);
            
        }else{
            getForkRight(idno);
            getForkLeft(idno);
            timesEaten = timesEaten+1;
            printf("Philosopher %i is eating %i times!\n", idno, timesEaten);
            usleep(microseconds);
            putForkLeft(idno);
            putForkRight(idno);
        }
    }

    
    return NULL;
}

int main(int argc, char* argv[])
{
    pthread_mutex_init(&mutex, NULL);

    int i = 0;
    int err;
    thread_number = 5;
    tid = malloc(thread_number * sizeof(pthread_t));
    printf("Creating %i threads\n", thread_number);
    for(i = 0; i < 5; i++){
        forks[i] = 0;
        pthread_cond_init(&conditions[i], NULL);
    }
    i = 0;

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