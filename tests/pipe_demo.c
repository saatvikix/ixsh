#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    // left child (ls)

    pid_t left_child = fork();

    if (left_child < 0) {
        perror("fork");
    }

    if (left_child == 0) {
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        close(pipefd[0]);
        close(pipefd[1]);

        char *args[] = {"ls", NULL};

        execvp(args[0], args);

        perror("execvp");
        exit(EXIT_FAILURE);
    }

    pid_t right_child = fork();

    if (right_child < 0) {
        perror("fork");
        return 1;
    }

    if (right_child == 0) {
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        close(pipefd[0]);
        close(pipefd[1]);

        char *args[] = {"grep", "src", NULL};
        execvp(args[0], args);

        perror("execvp");
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    
    waitpid(left_child, NULL, 0);
    waitpid(right_child, NULL, 0);

    return 0;
}