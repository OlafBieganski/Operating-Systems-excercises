#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void SIGTERM_HAND(int signum){
    printf("Program terminated.\n");
    exit(1);
}

void SIGUSR1_HAND(int signum){
    printf("Program will continue.\n");
}

void SIGUSR2_HAND(int signum){
    printf("Signal SIGUSR2 received.\n");
}

int main(){
    int i = 0;

    struct timespec rmtp, rqtp;
    rqtp.tv_sec = 0;
    rqtp.tv_nsec = 10000000;

    signal(SIGALRM, SIG_IGN); // ignorowanie sygnały SIGALRM
    signal(SIGTERM, SIGTERM_HAND); // reakcja na SIGTERM
    signal(SIGUSR1, SIGUSR1_HAND); // reakcja na SIGUSR1
    signal(SIGUSR2, SIGUSR2_HAND); // reakcja na SIGUSR2

    sigset_t intmask;
    if ((sigemptyset(&intmask) == -1) || (sigaddset(&intmask, SIGUSR2) == -1)){
    perror("Failed to initialize the signal mask");
    return 1;
    }
    printf("Blocking SIGUSR2.\n");
    sigprocmask(SIG_BLOCK, &intmask, NULL);
    while(1){
        i++;
        nanosleep(&rqtp, &rmtp);
        if(i % 1000 == 0){ // co 1000 iteracji na chwile odblokowujemy sygnał SIGUSR2
            sigprocmask(SIG_UNBLOCK, &intmask, NULL);
            printf("Signal SIGUSR2 unblocked.\n");
            sigprocmask(SIG_BLOCK, &intmask, NULL);
            printf("Blocking SIGUSR2.\n");
        }
    }
}