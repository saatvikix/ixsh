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

        // Parse command or pipeline
        Pipeline pipeline = parse_pipeline(buffer);

        // couldn't parse the left command
        if (pipeline.left.argv == NULL)
        {
            printf("Failed to parse command!\n");
            continue;
        }

        // pipeline
        if (pipeline.has_pipe)
        {
            if (pipeline.right.argv == NULL)
            {
                printf("Failed to parse pipeline!\n");

                free(pipeline.left.argv);
                continue;
            }

            execute_pipeline(
                &pipeline.left,
                &pipeline.right
            );

            free(pipeline.left.argv);
            free(pipeline.right.argv);

            continue;
        }

        // normal command

        Command command = pipeline.left;

        // built-in command
        if (handle_builtin(&command) == 1)
        {
            free(command.argv);
            continue;
        }

        // external command
        execute_command(&command);

        free(command.argv);
    }

    return 0;
}