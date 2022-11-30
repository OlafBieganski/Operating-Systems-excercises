#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv){
    char buff[10];
    int fifo_id;
    char *fifoName = "./fifo";
    int count;
    if ((fifo_id = open(fifoName, O_RDONLY)) < 0){
        perror("open():");
        exit(1);
    }
    while ((count = read(fifo_id, buff, 10)) > 0){
        write(STDOUT_FILENO, buff, count);
    }
    unlink(fifoName);
    return 0;
}