#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sched.h>
#include <sys/utsname.h>
#include <sys/times.h>
#include <sys/resource.h>



const int stack_size = 64;
static int *counter;

int dodaj(){
    *counter = *counter + 1;

    return 0;
}

double testFork(int N){
    struct tms tmschild;
    double child = 0;
    clock_t start, end;
    pid_t childPID;
    int i;
    for(i = 0; i < N; i++){
        start = times(&tmschild);
        if((childPID = fork()) == -1){
            printf("Error forking in fork!\n");
        }
        else if(childPID == 0)
        {
            *counter = *counter + 1;
            _exit(EXIT_SUCCESS);
        }else{
            int returnStatus;
            waitpid(childPID, &returnStatus, 0);
            end = times(&tmschild);
            child += (double) (end - start);
            if(returnStatus != EXIT_SUCCESS){
                printf("Child process returned with an error!\n");
                return -1;
            }
        }
    }
    return child;
}

double testvFork(int N){
    struct tms tmschild;
    double child = 0;
    clock_t start, end;
    pid_t childPID;
    int i;
    for(i = 0; i < N; i++){
        start = times(&tmschild);
        if((childPID = vfork()) == -1){
            printf("Error forking in vfork!\n");
            return -1;
        }
        else if(childPID == 0)
        {
            *counter = *counter + 1;
            _exit(EXIT_SUCCESS);
        }else{
            int returnStatus;
            waitpid(childPID, &returnStatus, 0);
            end = times(&tmschild);
            child += (double) (end - start);
            if(returnStatus != EXIT_SUCCESS){
                printf("Child process returned with an error!\n");
                return -1;
            }
        }
    }
    return child;
}


double testClone(int N){
    struct tms tmschild;
    double child = 0;
    clock_t start, end;
    pid_t PID;
    void *stack = malloc(stack_size);
    int i;
    for(i = 0; i < N; i++){
        start = times(&tmschild);
        PID = clone(dodaj, stack+stack_size, SIGCHLD, NULL);
        if(PID < 0){
            printf("Blad przy clone!\n");
            return -1;
        }
        int returnStatus;
        waitpid(PID, &returnStatus, 0);
        end = times(&tmschild);
            child += (double) (end - start);
        if(returnStatus != EXIT_SUCCESS){
            printf("Child process returned with an error!\n");
            return -1;
        }
    }
    free(stack);
    return child;
}

double testvClone(int N){
    struct tms tmschild;
    double child = 0;
    clock_t start, end;
    pid_t PID;
    void *stack = malloc(stack_size);
    int i;
    for(i = 0; i < N; i++){
        start = times(&tmschild);
        PID = clone(dodaj, stack+stack_size, SIGCHLD|CLONE_VM|CLONE_VFORK, NULL);
        if(PID == -1){
            printf("Blad przy clone!\n");
            return -1;
        }
        int returnStatus;
        waitpid(PID, &returnStatus, 0);
        end = times(&tmschild);
            child += (double) (end - start);
        if(returnStatus != EXIT_SUCCESS){
            printf("Child process returned with an error!\n");
            return -1;
        }
    }
    free(stack);
    return child;
}

static void print_times(int N, FILE *output, int type, clock_t real, struct tms *tmsstart, struct tms *tmsend, double child){
    static long clktck = 0;
    char *whatType = malloc(sizeof(char)*7);
    if(clktck == 0)
        if((clktck = sysconf(_SC_CLK_TCK)) < 0)
            printf("Getting clktck error!\n");
    if(type == 1) whatType = "fork";
    else if(type == 2) whatType = "vfork";
    else if(type == 3) whatType = "clone";
    else if(type == 4) whatType = "clonev";
        
    fprintf(output, "%i,real,%s,%f\n", N, whatType, real / (double) clktck);
    fprintf(output, "%i,user,%s,%f\n", N, whatType, (tmsend->tms_utime - tmsstart->tms_utime) / (double) clktck);
    fprintf(output, "%i,system,%s,%f\n", N, whatType, (tmsend->tms_stime - tmsstart->tms_stime) / (double) clktck);
    fprintf(output, "%i,user+system,%s,%f\n", N, whatType, (tmsend->tms_utime - tmsstart->tms_utime + tmsend->tms_stime - tmsstart->tms_stime)/ (double) clktck);
    fprintf(output, "%i,child_real,%s,%f\n", N, whatType, child / (double) clktck);
    fprintf(output, "%i,child_user,%s,%f\n", N, whatType, (tmsend->tms_cutime - tmsstart->tms_cutime) / (double) clktck);
    fprintf(output, "%i,child_system,%s,%f\n", N, whatType, (tmsend->tms_cstime - tmsstart->tms_cstime) / (double) clktck);
    fprintf(output, "%i,child_user+system,%s,%f\n", N, whatType, (tmsend->tms_cutime - tmsstart->tms_cutime + tmsend->tms_cstime - tmsstart->tms_cstime)/ (double) clktck);
    fprintf(output, "%i,parent+child_real,%s,%f\n", N, whatType, (real + child) / (double) clktck);
    fprintf(output, "%i,parent+child_user,%s,%f\n", N, whatType, ((tmsend->tms_utime - tmsstart->tms_utime) + (tmsend->tms_cutime - tmsstart->tms_cutime)) / (double) clktck);
    fprintf(output, "%i,parent+child_system,%s,%f\n", N, whatType, ((tmsend->tms_stime - tmsstart->tms_stime) + (tmsend->tms_cstime - tmsstart->tms_cstime)) / (double) clktck);
    fprintf(output, "%i,parent+child_user+system,%s,%f\n", N, whatType, ((tmsend->tms_utime - tmsstart->tms_utime + tmsend->tms_stime - tmsstart->tms_stime) + (tmsend->tms_cutime - tmsstart->tms_cutime + tmsend->tms_cstime - tmsstart->tms_cstime)) / (double) clktck);
    // free(whatType);
    return;
}

int main(int argc, char* argv[]){
    
    FILE *results;
    
    results = fopen("results.txt", "w");
    
    fprintf(results, "N,type,algo,time\n");
    
    int N, i;
    
    counter = mmap(NULL, sizeof *counter, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *counter = 0;
    
    struct tms tmsstart, tmsend;
    clock_t start, end;
    double child = 0;

    for(i = 1; i < argc; i++){
        N = atoi(argv[i]);
        printf("Computing: %i\n", N);
        if(((start = times(&tmsstart)) == -1)) printf("Error starting time\n");
        child = testFork(N);
        if(((end = times(&tmsend)) == -1)) printf("Error ending times\n");
        print_times(N, results, 1, end-start,&tmsstart,&tmsend, child);

        if(((start = times(&tmsstart)) == -1)) printf("Error starting time\n");
        child = testvFork(N);
        if(((end = times(&tmsend)) == -1)) printf("Error ending times\n");
        print_times(N, results, 2, end-start,&tmsstart,&tmsend, child);
        
        if(((start = times(&tmsstart)) == -1)) printf("Error starting time\n");
        child = testClone(N);
        if(((end = times(&tmsend)) == -1)) printf("Error ending times\n");
        print_times(N, results, 3, end-start,&tmsstart,&tmsend, child);
        
        if(((start = times(&tmsstart)) == -1)) printf("Error starting time\n");
        child = testvClone(N);
        if(((end = times(&tmsend)) == -1)) printf("Error ending times\n");
        print_times(N, results, 4, end-start,&tmsstart,&tmsend, child);
               
    } 
    printf("The final counter value is: %i\n", *counter);
    munmap(counter, sizeof *counter);
    return 0;
}