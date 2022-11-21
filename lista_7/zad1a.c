#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char* argv[]){
    // check if argument is present
    if(argc < 2){
        printf("Missing argument.\n");
        exit(1);
    }
    // open a file given as an argument
    const char* file_name = argv[1];
    int fd = open(file_name, O_RDWR);
    // get a size of the file
    struct stat data;
    if(fstat(fd, &data) != 0){
        perror("fstat: ");
        exit(2);
    }
    uint file_size = data.st_size;
    char* m_area = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(m_area == MAP_FAILED){
        perror("mmap: ");
        exit(3);
    }
    // get a user input to the file
    char* usr_in = calloc(file_size, sizeof(char));
    printf("Provide input to the file (max %d characters): ", file_size-1);
    if(fgets(usr_in, file_size, stdin) == NULL){
        printf("fgets error.");
        exit(4);
    }
    // swap file content with new starting from the beginning
    uint lenght = strlen(usr_in);
    memcpy(m_area, usr_in, lenght);
    if(msync(m_area, file_size, MS_SYNC) != 0 ){
        perror("msync(): ");
        exit(5);
    }
    if(munmap(m_area, file_size) != 0){
        perror("munmap(): ");
        exit(5);
    }
    close(fd);
    return 0;
}