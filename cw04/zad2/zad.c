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
        // printf("Amounts of signals recieved %li", counter);
    }else{
        printf("Recieved strange signal: %d\n", signo);
    }
    return;
}
static void sig_user2(int signo){
    if(signo == SIGUSR2){
        // printf("Recieved signal SIGUSR2\n");
        end = true;
    }else{
        printf("Recieved strange signal: %d\n", signo);
    }
}


int main(int argc, char* argv[]){
    long int signalAmount = atoi(argv[1]);
    if(signalAmount < 1) printf("Wrong argument!\n");
	pid_t childPID;

	sigset_t set;
	sigemptyset (&set);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGUSR2);  
	if(sigprocmask(SIG_BLOCK, &set, NULL) !=0){
        printf("Error setting mask!\n");
        return 1;
    }
	// Ustawienie obslugi sygnalow

	struct sigaction usr1Action, usr2Action;
	usr1Action.sa_handler = sig_user1;
	usr1Action.sa_mask = set;
	usr2Action.sa_handler = sig_user2;
	usr2Action.sa_mask = set;
	
	if (sigaction(SIGUSR1, &usr1Action, NULL) < 0) {
		fprintf(stderr, "Blad przy ustawianiu procedury obslugi sygnalu!\n");
		exit(-1);			
	}
	if (sigaction(SIGUSR2, &usr2Action, NULL) < 0 ) {
		fprintf(stderr, "Blad przy ustawianiu procedury obslugi sygnalu!\n");	
		exit(-1);		
	}

	childPID = fork();
		if(childPID == -1){
            printf("Error forking in fork!\n");
        }else if(childPID == 0){
        	// printf("Child created\n");
            pid_t parentPID = getppid(); //shall always return ppid, no errors
            sigemptyset(&set);
            while(end == false){
                sigsuspend(&set);
                // printf(" in child\n");
                if(kill(parentPID, SIGUSR1) != 0){
                	printf("Error sending SIGUSR1 from child!\n");
                }
            }
            
            if(kill(parentPID, SIGUSR2) != 0){
                printf("Error sending SIGUSR2 from child!\n");
            }
            //child
            // printf("Ending child! Goodbye!\n");
            _exit(EXIT_SUCCESS);
        }else{
        	// printf("Parent is sending to %i\n", (int) childPID);
            int i;
            sigemptyset(&set);
            sigaddset(&set, SIGUSR2);
        	for(i = 0; i < signalAmount; i++){
        		if(kill(childPID, SIGUSR1) != 0){
        			printf("Error sending SIGUSR1 from parent!\n");
        		}
        		sigsuspend(&set);
        		// printf(" in parent\n");
        	}
        	sigdelset(&set, SIGUSR2);
        	sigaddset(&set, SIGUSR1);
            if(kill(childPID, SIGUSR2) != 0){
                printf("Error sending SIGUSR2 from parent!\n");
            }
            // printf("Amounts of confirmations recieved from child: %li\n", counter);
            while(end == false){
               sigsuspend(&set);
            }
            printf("Amount of signals that were sent to child: %li\n", signalAmount);
            printf("Amount of signal recieved from child: %li\n", counter);
            // parent;
        }

	return 0;
}
