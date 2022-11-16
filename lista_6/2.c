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
    char *s_buffer;
    size_t b_size = 0;
    ssize_t c_size = 0;
    int fpid;

    char *path = "/etc/alternatives/display";
    char *args[] = {path, STDIN_FILENO, NULL};
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
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO); // does same as dup & close in one command
        execvp(path, args);
        close(pipefd[0]);
        _exit(EXIT_SUCCESS); // specific exit for child process that doesnt flush io buffers and does not call atexit();
    }
    else // parent
    {
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
        }
        close(pipefd[1]);
        wait(NULL);
        exit(EXIT_SUCCESS);
    }
    return EXIT_SUCCESS;
}