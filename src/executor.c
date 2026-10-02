#include "executor.h"
#include "redirection.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>

void execute_command(Command *command)
{

	pid_t pid = fork();

	if (pid < 0)
	{
		perror("fork");
		return;
	}

	// child process
	if (pid == 0)
	{
		if (apply_redirection(command) < 0)
		{
			exit(EXIT_FAILURE);
		}

		execvp(command->argv[0], command->argv);

		perror("execvp");
		exit(EXIT_FAILURE);
	}

	// parent process waits till the child gets completed
	wait(NULL);
}

void execute_pipeline(Command *left, Command *right)
{
    int pipefd[2];

    if (pipe(pipefd) < 0)
    {
        perror("pipe");
        return;
    }

   // left child
    pid_t left_pid = fork();

    if (left_pid < 0)
    {
        perror("fork");

        close(pipefd[0]);
        close(pipefd[1]);

        return;
    }

    if (left_pid == 0)
    {
        // stdout -> pipe write end
        if (dup2(pipefd[1], STDOUT_FILENO) < 0)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        close(pipefd[0]);
        close(pipefd[1]);

        // Allow redirection on the left command too
        if (apply_redirection(left) < 0)
        {
            exit(EXIT_FAILURE);
        }

        execvp(left->argv[0], left->argv);

        perror("execvp");
        exit(EXIT_FAILURE);
    }

    // right child
    pid_t right_pid = fork();

    if (right_pid < 0)
    {
        perror("fork");

        close(pipefd[0]);
        close(pipefd[1]);

        waitpid(left_pid, NULL, 0);

        return;
    }

    if (right_pid == 0)
    {
        // stdin <- pipe read end
        if (dup2(pipefd[0], STDIN_FILENO) < 0)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        close(pipefd[0]);
        close(pipefd[1]);

        // Allow redirection on the right command too
        if (apply_redirection(right) < 0)
        {
            exit(EXIT_FAILURE);
        }

        execvp(right->argv[0], right->argv);

        perror("execvp");
        exit(EXIT_FAILURE);
    }

    // parent process
	
    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(left_pid, NULL, 0);
    waitpid(right_pid, NULL, 0);
}