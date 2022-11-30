#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define ITER 1000000000

double sumPI = 0.0;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void *computePI(ulong nr){
    double x, y, pi;
    double square = 0.0;
    double circle4 = 0.0;
    for(int i = 0; i < ITER; i++){
        x = drand48();
        y = drand48();
        if(y < sqrt(1.0 - x*x)) circle4++;
        square++;
    }
    pi = 4*(circle4/square);
    printf("PI calculeted by thread nr %ld is: %f\n", nr, pi);
    pthread_mutex_lock(&mut);
    sumPI += pi;
    pthread_mutex_unlock(&mut);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    if(argc < 2){
        printf("Brak argumentu.\n");
        exit(1);
    }
    // converting argument to long
    ulong t_num = strtoul(argv[1], NULL, 0);
    pthread_t threads[t_num];
    int rc;
    // creating threads
    for(ulong i = 0; i < t_num; i++){
        rc = pthread_create(&threads[i], NULL,(void*) computePI,(void*) i);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for(ulong i = 0; i < t_num; i++){
        pthread_join(threads[i], NULL);
    }

    printf("PI calculated as avreage = %f\n", sumPI / (double) t_num);
    pthread_exit(NULL);
}