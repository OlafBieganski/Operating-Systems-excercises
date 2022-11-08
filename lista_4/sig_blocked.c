#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void SIGTERM_HAND(int signum){
    printf("Program will continue.\n");
}

int main(){
    int i = 0;

    struct timespec rmtp, rqtp;
    rqtp.tv_sec = 0;
    rqtp.tv_nsec = 10000000;

    signal(SIGILL, SIG_IGN); // ignorowanie sygnały SIGILL
    signal(SIGTERM, SIGTERM_HAND); // reakcja na SIGTERM

    sigset_t intmask;
    sigemptyset(&intmask);
    sigaddset(&intmask, SIGUSR2);
    sigaddset(&intmask, SIGUSR1);
    sigaddset(&intmask, SIGINT);
    sigaddset(&intmask, SIGALRM);
    sigprocmask(SIG_BLOCK, &intmask, NULL);
    while(1){
        i++;
        nanosleep(&rqtp, &rmtp);
    }
}