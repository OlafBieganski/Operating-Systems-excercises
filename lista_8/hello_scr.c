#include <pthread.h>
#include <stdio.h>


void* routine(void* count){
    pthread_t self;
    self = pthread_self();
    printf("Hello SCR. Written by thread %ld. (nr %ld)\n", self, (long)count);
    pthread_exit(0);
}

int main(){
    pthread_t t1,t2,t3;
    if(pthread_create(&t1, NULL, &routine, (void *)1) != 0) return 1;
    if(pthread_create(&t2, NULL, &routine, (void *)2) != 0) return 1;
    if(pthread_create(&t3, NULL, &routine, (void *)3) != 0) return 1;
    if(pthread_join(t1, NULL)) return 1;
    if(pthread_join(t2, NULL)) return 1;
    if(pthread_join(t3, NULL)) return 1;
    pthread_exit(0);
}