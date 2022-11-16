#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
    FILE *file;
    char *filename;
    int pipefd[2];
    char buffer;
    char *s_buffer;
    size_t b_size = 0;
    ssize_t c_size = 0;
    int fpid;
    if (pipe(pipefd) == -1)
    {
        perror("Pipe intialization failure!\n");
        exit(EXIT_FAILURE);
    }

    if (argc == 1)
    {
        perror("Argument 1 not specified. Filename required!\n");
        exit(EXIT_FAILURE);
    }
    filename = argv[1];

    fpid = fork();
    if (fpid == -1) // error
    {
        perror("Couldn't fork process!\n");
        exit(EXIT_FAILURE);
    }
    if (fpid == 0) // child
    {
        printf("\nIm a child!\n");
        close(pipefd[1]);
        while ((c_size = read(pipefd[0], &buffer, 10)) > 0)
        {
            write(STDOUT_FILENO, "#", 1);
            write(STDOUT_FILENO, &buffer, c_size);
            write(STDOUT_FILENO, "#", 1);
            write(STDOUT_FILENO, "\n", 1);
        }
        close(pipefd[0]);
        _exit(EXIT_SUCCESS); // specific exit for child process that doesnt flush io buffers and does not call atexit();
    }
    else // parent
    {
        printf("\nIm a parent!\n");
        close(pipefd[0]);
        file = fopen(filename, "r+");
        if (file == NULL)
        {
            perror("Couldn't open file!\n");
            exit(EXIT_FAILURE);
        }
        while ((c_size = getline(&s_buffer, &b_size, file)) > 0)
        {
            write(pipefd[1], s_buffer, c_size);
            // write(STDOUT_FILENO, s_buffer, c_size);
        }
        // write(STDOUT_FILENO, "\n", 1);

        close(pipefd[1]);
        wait(NULL);
        exit(EXIT_SUCCESS);
    }
    return EXIT_SUCCESS;
}