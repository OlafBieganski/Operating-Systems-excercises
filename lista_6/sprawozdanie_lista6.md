# Sprawozdanie lista 6

### Zadanie 3

Poniższy plik prezentuje kod programu write_fifo.c służący do pisania do kolejki FIFO. Wpisuje on zawartość plików podanych jako argumenty podczas wywołania programu do kolejki.

```c
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
```

Poniższy plik prezentuje kod programu read_fifo.c służącego do czytania zawartości plików zapisanych w kolejce FIFO i wypisaywania ich zawartości na STDOUT.

```c
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
```