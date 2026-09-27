#include "builtins.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>

// 1: This command was a built in. It's been handeled
// 0: The command wasn't a built-in. Need to handle it.
int handle_builtin(Command *command) {
    
    if (command->argc == 0) {
        return 1;
    }

    // cd
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

    // pwd

    if (strcmp(command->argv[0], "pwd") == 0) {

        char cwd[1024];

        if(getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("pwd");
        }

        else {
            printf("%s\n", cwd);
        }

        return 1;
    }

    // help
    if (strcmp(command->argv[0], "help") == 0) {
        printf(" =-=-=-=-=-=-= ixsh help =-=-=-=-=-=-=-=\n\n");
        printf("cd:               Change directory\n");
        printf("pwd:              Print current directory\n");
        printf("help:             View this manual\n");
        printf("exit:             Exit ixsh\n");

        return 1;
    }

    return 0;
}