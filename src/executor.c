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

    // Wait for the child
    wait(NULL);
}

void execute_pipeline(Pipeline *pipeline)
{
    int command_count = pipeline->count;

    // (N-1) pipes are needed for N commands
    int pipes[command_count - 1][2];

    // Store child PIDs so the parent can wait for them
    pid_t pids[command_count];


    // Create the pipes before forking
    for (int i = 0; i < command_count - 1; i++)
    {
        if (pipe(pipes[i]) < 0)
        {
            perror("pipe");
            return;
        }
    }

    // Start one child for each command
    for (int i = 0; i < command_count; i++)
    {
        pids[i] = fork();

        if (pids[i] < 0)
        {
            perror("fork");
            return;
        }

        // Child process
        if (pids[i] == 0)
        {
            // Connect stdin to the previous pipe
            if (i > 0)
            {
                if (dup2(pipes[i - 1][0], STDIN_FILENO) < 0)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Connect stdout to the next pipe
            if (i < command_count - 1)
            {
                if (dup2(pipes[i][1], STDOUT_FILENO) < 0)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Close the original pipe descriptors
            for (int j = 0; j < command_count - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            Command *command = &pipeline->commands[i];

            // Run built-ins inside the pipeline child
            if (handle_builtin(command) == 1)
            {
                exit(EXIT_SUCCESS);
            }

            // Apply redirection after connecting pipes
            if (apply_redirection(command) < 0)
            {
                exit(EXIT_FAILURE);
            }

            // Run an external command
            execvp(command->argv[0], command->argv);

            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    // Close pipe descriptors in the parent
    for (int i = 0; i < command_count - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all children
    for (int i = 0; i < command_count; i++)
    {
        waitpid(pids[i], NULL, 0);
    }
}
