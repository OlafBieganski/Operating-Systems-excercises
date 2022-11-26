#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define MAX_FSIZE 100

int main(int argc, char* argv[]){
    // check if argument is present
    if(argc < 2){
        printf("Missing argument.\n");
        exit(1);
    }
    // open a file given as an argument
    const char* file_name = argv[1];
    int fdW = open(file_name, O_RDWR);

    char buf[MAX_FSIZE];
    while(1){
        // code for user input
        printf("Type the name of the file you want to swap with argument file (or 'x' to end program): ");
        if(scanf("%s", buf) == EOF){
            printf("Incorrect input for scanf().\n");
            exit(3);
        }
        //check if program have to end
        if(buf[0] == 'x' /*&& buf[1] == '\n'*/) break;
        // open file 'READ' for read
        int fdR = open(buf, O_RDWR);
        if(fdR < 0){
            printf("%s", buf);
            perror("open(): ");
            exit(7);
        }
        struct stat dataR;
        // get the size of 'READ'file
        if(fstat(fdR, &dataR) != 0){
            perror("fstat: ");
            exit(4);
        }
        uint fdR_size = dataR.st_size;
        // change size of the 'WRITE' file
        if(ftruncate(fdW, fdR_size) != 0){
            perror("ftruncate(): ");
            exit(5);
        }
        // may be erased later cause now fdW == fdR
        struct stat dataW;
        if(fstat(fdW, &dataW) != 0){
            perror("fstat: ");
            exit(2);
        }
        uint fdW_size = dataW.st_size;
        //map data from file 'WRITE' to shared memory
        char* m_area = mmap(NULL, fdW_size, PROT_WRITE | PROT_READ, MAP_SHARED, fdW, 0);
        if(m_area == MAP_FAILED){
            perror("mmap: ");
            exit(6);
        }
        // copy data from file to shared memory
        if(read(fdR, m_area, fdR_size) < 0){
            perror("read(): ");
            exit(8);
        }
        // synchronize shared memory with 'WRITE' file
        if(msync(m_area, fdW_size, MS_SYNC) != 0 ){
            perror("msync(): ");
            exit(9);
        }
        if(munmap(m_area, fdW_size) != 0){
            perror("munmap(): ");
            exit(5);
        }
        close(fdR);
    }
    close(fdW);
    return 0;
}