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
int thread_number;

void* doSomeThing(void *arg)
{
    pthread_t id = pthread_self();

    int idno;
    for(idno = 0; idno < thread_number; idno++){
        if(pthread_equal(id,tid[idno])){
            break;
        }
    }
    
    while(1){
        printf("Thread %i sleeping!\n", idno);
        sleep(10);
        if(idno == 0){
            int a = 1 / 0;
            printf("%i\n", a);
        }
    }
    return NULL;
}
void sigfunc(int sig){
    pthread_t id = pthread_self();

    int idno;
    for(idno = 0; idno < thread_number; idno++){
        if(pthread_equal(id,tid[idno])){
            break;
        }
    }
    sleep(1);
    printf("Caught signal %i in thread %i\n", sig, idno);
}

int main(int argc, char* argv[])
{
    printf("PID: %i\n", (int)getpid());
    if(argc != 2){
        printf("Podano zla liczbe argumentow\n");
        return 0;
    }
    int err;
    int i = 0;
    thread_number = atoi(argv[1]);
    tid = malloc(thread_number * sizeof(pthread_t));
    // signal(SIGFPE, sigfunc);
    printf("Podaj wersje:\n");
    
    int version;
    scanf("%i", &version);
    printf("Creating %i threads\n", thread_number);
    while(i < thread_number)
    {
        err = pthread_create(&(tid[i]), NULL, &doSomeThing, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        else
            printf("\n Thread created successfully\n");
        i++;
    }
    if(version == 1){
        pthread_kill(tid[1], SIGUSR1);
    }else if(version == 2){
        pthread_kill(tid[1], SIGTERM);
    }else if(version == 3){
        pthread_kill(tid[1], SIGKILL);
    }else if(version == 4){
        pthread_kill(tid[1], SIGSTOP);
    }
    
    for(i = 0; i < thread_number; i++){
        pthread_join(tid[i], NULL);
    }

    // sleep(2);
    return 0;
}