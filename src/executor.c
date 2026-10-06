#include "executor.h"
#include "redirection.h"
#include "builtins.h"

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

    // Child process
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

    // Parent waits for child
    wait(NULL);
}

void execute_pipeline(Pipeline *pipeline)
{
    int command_count = pipeline->count;

    // (N-1) pipes are needed for N commands
    int pipes[command_count - 1][2];

    // storing pids for al child processes so the parent can wait fot them
    pid_t pids[command_count];


    // STEP 1: CREATE ALL PIPES
    for (int i = 0; i < command_count - 1; i++)
    {
        if (pipe(pipes[i]) < 0)
        {
            perror("pipe");
            return;
        }
    }

    // STEP 2: FORK ALL CHILDREN
    for (int i = 0; i < command_count; i++)
    {
        pids[i] = fork();

        if (pids[i] < 0)
        {
            perror("fork");
            return;
        }

        // CHILD PROCESS
        if (pids[i] == 0)
        {
            //  if this is not the first command, take input from the previous pipe
            if (i > 0)
            {
                if (dup2(pipes[i - 1][0], STDIN_FILENO) < 0)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // if this is not the last command, send output to the next pipe
            if (i < command_count - 1)
            {
                if (dup2(pipes[i][1], STDOUT_FILENO) < 0)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // close al original desciptors in this child
            for (int j = 0; j < command_count - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            Command *command = &pipeline->commands[i];

            // handling bulitins
            if (handle_builtin(command) == 1)
            {
                exit(EXIT_SUCCESS);
            }

            // applying redirections if needed
            if (apply_redirection(command) < 0)
            {
                exit(EXIT_FAILURE);
            }

            // external commands
            execvp(command->argv[0], command->argv);

            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    // STEP 3: PARENT CLOSES ALL PIPES
    for (int i = 0; i < command_count - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // STEP 4: PARENT WAITS FOR CHILDREN
    for (int i = 0; i < command_count; i++)
    {
        waitpid(pids[i], NULL, 0);
    }
}