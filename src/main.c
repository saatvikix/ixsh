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

        // input like CTRL+Z
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("couldn't handle input\n");
            break;
        }

        // remove newline
        buffer[strcspn(buffer, "\n")] = '\0';

        // break the terminal loop
        if (strcmp(buffer, "exit") == 0)
        {
            break;
        }

        Command command = parse_command(buffer);

        if (command.argv == NULL)
        {
            printf("Failed to parse the command! \n");
            continue;
        }

        if (handle_builtin(&command) == 1)
        {
            free(command.argv);
            continue;
        }

        execute_command(&command);

        free(command.argv);
    }

    return 0;
}
