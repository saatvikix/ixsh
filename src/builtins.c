#include "builtins.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>

// 1: This command was a built in. It's been handeled
// 0: The command wasn't a built-in. Need to handle it.
int handle_builtin(Command *command) {
    
    if (strcmp(command->argv[0], "cd") == 0) {

        if (command->argc < 2) {
            printf("Invalid usage of cd: Missing arguments\n");
            return 1;
        }

        if (chdir(command->argv[1]) != 0) {
            perror("cd");
        }

        return 1; 
    }

    return 0;
}