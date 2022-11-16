#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define PIPE_ERR 1
#define FORK_ERR 2
#define ARG_ERR 3
#define FILE_ERR 4

int main(int argc, char* argv[]){
    if(argc != 2) return ARG_ERR;
    int pipe_fd[2];
    if(pipe(pipe_fd) < 0) return PIPE_ERR;
    int pid = fork();
    if(pid == -1) return FORK_ERR;
    if(pid == 0){
        // Child process
        close(pipe_fd[1]);
	char* prog_name = "/etc/alternatives/display";
	char* args[] = {"/etc/alternatives/display", STDIN_FILENO, NULL};
        dup2(pipe_fd[0], STDIN_FILENO);
	execv(prog_name, args);
        close(pipe_fd[0]);
        exit(0);
    } else {
        // parent process
        close(pipe_fd[0]);
        FILE *file = fopen(argv[1], "r");
        if(file == NULL) return FILE_ERR;
        char buf;
        while(1){
            buf = fgetc(file);
            if(write(pipe_fd[1], &buf, 1) != 1) perror("Error: ");
            if(feof(file)) break;
        }
        close(pipe_fd[1]);
        wait(NULL);
        exit(0);
    }
}
