#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv){

    FILE *file;
    char *fifoName = "./fifo", *filename;
    char *buff;
    size_t b_size = 0;
    ssize_t c_size = 0;
    int fifo_id;

    if (mkfifo(fifoName, 0666) != 0){
        perror("mkfifo():");
        exit(1);
    }
    if ((fifo_id = open(fifoName, O_WRONLY)) < 0){
        perror("open():");
        exit(1);
    }
    if (argc == 0){
        perror("No argument.\n");
        exit(1);
    }
    for (int i = 1; i < argc; i++){
        filename = argv[i];
        if ((file = fopen(filename, "r+")) == NULL){
            perror("fopen():");
            exit(1);
        }
        while ((c_size = getline(&buff, &b_size, file)) > 0){
            write(fifo_id, buff, c_size);
        }
        fclose(file);
    }
    return 0;
}