#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "parser.h"
#include "builtins.h"
#include "executor.h"

int main(void)
{
    printf("Welcome to ixsh!\n");

    while (true)
    {
        char buffer[1024];
        char cwd[1024];

        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            perror("getcwd");
            strcpy(cwd, "?");
        }

        printf("ixsh:%s> ", cwd);
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("\nExiting ixsh...\n");
            break;
        }

        // Remove newline
        buffer[strcspn(buffer, "\n")] = '\0';

        // Exit shell
        if (strcmp(buffer, "exit") == 0)
        {
            break;
        }

        // Parse either one command or multiple piped commands
        Pipeline pipeline = parse_pipeline(buffer);

        if (pipeline.commands == NULL)
        {
            printf("Failed to parse command!\n");
            continue;
        }

        // ---------------------------------
        // EMPTY INPUT
        // ---------------------------------
        if (pipeline.count == 1 &&
            pipeline.commands[0].argc == 0)
        {
            free_pipeline(&pipeline);
            continue;
        }

        // ---------------------------------
        // VALIDATE PIPELINE
        // ---------------------------------
        int invalid_pipeline = 0;

        if (pipeline.count > 1)
        {
            for (int i = 0; i < pipeline.count; i++)
            {
                if (pipeline.commands[i].argc == 0)
                {
                    invalid_pipeline = 1;
                    break;
                }
            }
        }

        if (invalid_pipeline)
        {
            printf(
                "Invalid pipeline: missing command\n"
            );

            free_pipeline(&pipeline);
            continue;
        }

        // ---------------------------------
        // MULTIPLE COMMANDS -> PIPELINE
        // ---------------------------------
        if (pipeline.count > 1)
        {
            execute_pipeline(&pipeline);

            free_pipeline(&pipeline);
            continue;
        }

        // ---------------------------------
        // SINGLE COMMAND
        // ---------------------------------
        Command *command = &pipeline.commands[0];

        // Built-in command
        if (handle_builtin(command) == 1)
        {
            free_pipeline(&pipeline);
            continue;
        }

        // External command
        execute_command(command);

        free_pipeline(&pipeline);
    }

    return 0;
}