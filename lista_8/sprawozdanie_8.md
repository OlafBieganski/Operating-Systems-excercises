# Sprawozdanie - lista 8

## Zadanie 4

Poniższy kod prezentuje propzycje implemantacji algorytmu estymacji liczby pi przy użyciu metody Monte Carlo. Poszczególne fragmenty algolrytmu zostały  opisane komentarzami w kodzie.

```c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define ITER 1000000000 // number of iteration

double sumPI = 0.0;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void *computePI(ulong nr){
    double x, y, pi;
    double square = 0.0; // how many in sum
    double circle4 = 0.0; // how many points in circle area
    // with each iteration we draw rnadom number
    for(int i = 0; i < ITER; i++){
        x = drand48();
        y = drand48();
        // and check if in the circle area or not
        if(y < sqrt(1.0 - x*x)) circle4++;
        square++;
    }
    pi = 4*(circle4/square); // estimation of pi
    printf("PI calculeted by thread nr %ld is: %f\n", nr, pi);
    // we have to protect write beacause many threads run at the same time
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
    // waiting for threads to finish its job
    for(ulong i = 0; i < t_num; i++){
        pthread_join(threads[i], NULL);
    }
    // calculating and printing average pi estimation
    printf("PI calculated as avreage = %f\n", sumPI / (double) t_num);
    pthread_exit(NULL);
}
```