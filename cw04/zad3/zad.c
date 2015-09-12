#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sched.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>
#include <errno.h>

bool end = false;
bool qFull = false;
long int counter = 0;
static void sig_user1(int signo){
	if(signo == SIGRTMIN+1){
        ++counter;
        // printf("Amounts of signals recieved %li", counter);
    }else{
        printf("Recieved strange signal: %d\n", signo);
    }
    return;
}
static void sig_user2(int signo){
    if(signo == SIGRTMIN+2){
        if(qFull) printf("Result is incorrect due to overflowing queue\n");
        end = true;
    }else{
        printf("Recieved strange signal: %d\n", signo);
    }
}


int main(int argc, char* argv[]){
    long int signalAmount = atoi(argv[1]);
    if(signalAmount < 1) printf("Wrong argument!\n");
    // int sleepTime = 1;
	pid_t childPID;

	sigset_t set;
	sigemptyset (&set);
	sigaddset(&set, SIGRTMIN + 1);
	sigaddset(&set, SIGRTMIN + 2);
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
	
	if (sigaction(SIGRTMIN+1, &usr1Action, NULL) < 0) {
		fprintf(stderr, "Blad przy ustawianiu procedury obslugi sygnalu!\n");
		exit(-1);			
	}
	if (sigaction(SIGRTMIN+2, &usr2Action, NULL) < 0 ) {
		fprintf(stderr, "Blad przy ustawianiu procedury obslugi sygnalu!\n");	
		exit(-1);		
	}
    union sigval Sigval;
	childPID = fork();
		if(childPID == -1){
            printf("Error forking in fork!\n");
        }else if(childPID == 0){
        	// printf("Child created\n");
            pid_t parentPID = getppid(); //shall always return ppid, no errors
            sigemptyset(&set);
            while(end == false && qFull == false){
                sigsuspend(&set);
                // printf(" in child\n");
            }
            int i;
            for(i = 0; i < counter; i++){
                if(sigqueue(parentPID, SIGRTMIN + 1, Sigval) < 0){
                    if(errno == EAGAIN){
                        qFull = true;
                    }else{
                         printf("Error sending sig1 from child!\n");
                         i = counter;
                    }
                }
            }
            if(sigqueue(parentPID, SIGRTMIN + 2, Sigval) < 0){
                    if(errno == EAGAIN){
                        qFull = true;
                    }else{
                         printf("Error sending sig2 from child!\n");
                    }
                }
            //child
            // printf("Ending child! Goodbye!\n");
            _exit(EXIT_SUCCESS);
        }else{
        	// printf("Parent is sending to %i\n", (int) childPID);
            int i;
            sigemptyset(&set);
            sigaddset(&set, SIGRTMIN+2);
        	for(i = 0; i < signalAmount; i++){
        		if(sigqueue(childPID, SIGRTMIN + 1, Sigval) < 0){
                    if(errno == EAGAIN){
                        qFull = true;
                    }else{
        			     printf("Error sending sig1 from parent!\n");
                    }
        		}
            }
            sigdelset(&set, SIGRTMIN+2);
            if(sigqueue(childPID, SIGRTMIN + 2, Sigval) < 0){
                    if(errno == EAGAIN){
                        qFull = true;
                    }else{
                         printf("Error sending sig1 from parent!\n");
                    }
                }
            while(end == false && qFull == false){
               sigsuspend(&set);
        		// printf(" in parent\n");
            }
            printf("Amount of signals that were sent to child: %li\n", signalAmount);
            printf("Amount of signal recieved from child: %li\n", counter);
            // parent;
        }

	return 0;
}



