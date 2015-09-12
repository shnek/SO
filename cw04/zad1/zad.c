#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sched.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>


bool end = false;
long int counter = 0;
static void sig_user1(int signo){
	if(signo == SIGUSR1){
        ++counter;
        // printf("Amounts of signals recieved: %li\n", counter);
    }else{
        printf("Recieved strange signal: %d\n", signo);
    }
    return;
}
static void sig_user2(int signo){
    if(signo == SIGUSR2){
        printf("Recieved signal SIGUSR2\n");
        end = true;
    }else{
        printf("Recieved strange signal: %d\n", signo);
    }
    return;
}


int main(int argc, char* argv[]){
    long int signalAmount = atoi(argv[1]);
    if(signalAmount < 1) printf("Wrong argument!\n");
    int sleepTime = 100;
	pid_t childPID;
	childPID = fork();
		if(childPID == -1){
            printf("Error forking in fork!\n");
        }else if(childPID == 0){
        	// printf("Child created\n");
			while(end == false){
				// printf("child is alive!\n");
				if (signal(SIGUSR1, sig_user1) == SIG_ERR) printf("Error catching signal 1 in child\n");	
                if (signal(SIGUSR2, sig_user2) == SIG_ERR) printf("Error catching signal 2 in child\n");
				//sleep(0.5);
			}
            pid_t parentPID = getppid(); //shall always return ppid, no errors
            // printf("Parent pid %i\n", getppid());
            int i;
            usleep(sleepTime);
            for(i = 0; i < counter; i++){
                if(kill(parentPID, SIGUSR1) != 0){
                    printf("Error sending SIGUSR1 from child!\n");
                }
            }
            usleep(sleepTime);
            if(kill(parentPID, SIGUSR2) != 0){
                printf("Error sending SIGUSR2 from child!\n");
            }
            // printf("Ending child! Goodbye!\n");
            _exit(EXIT_SUCCESS);
        }else{
        	// printf("Parent is sending to %i\n", (int) childPID);
        	usleep(sleepTime);
            int i;
        	for(i = 0; i < signalAmount; i++){
        		if(kill(childPID, SIGUSR1) != 0){
        			printf("Error sending SIGUSR1 from parent!\n");
        		}
            }
            usleep(sleepTime);
            if(kill(childPID, SIGUSR2) != 0){
                printf("Error sending SIGUSR2 from parent!\n");
            }
            while(end == false){
                if (signal(SIGUSR1, sig_user1) == SIG_ERR) printf("Error catching signal 1 in parent\n");    
                if (signal(SIGUSR2, sig_user2) == SIG_ERR) printf("Error catching signal 2 in parent\n");
                //sleep(0.5);
            }
            printf("Amount of signals that were sent to child: %li\n", signalAmount);
            printf("Amount of signal recieved from child: %li\n", counter);
        }

	return 0;
}