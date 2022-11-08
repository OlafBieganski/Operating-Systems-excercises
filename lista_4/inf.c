#include <time.h>
#include<stdio.h>

int main(){
    int i = 0;
    struct timespec rmtp, rqtp;
    rqtp.tv_sec = 0;
    rqtp.tv_nsec = 10000000;
    while(1){
        i++;
        //nanosleep(&rqtp, &rmtp);
    }
}