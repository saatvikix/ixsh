#include "builtins.h"
#include "redirection.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>

static int is_builtin(Command *command)
{
    if (command->argc == 0) 
    {
        return 0;
    }

    return strcmp(command->argv[0], "pwd") == 0 ||
           strcmp(command->argv[0], "cd") == 0 ||
           strcmp(command->argv[0], "help") == 0;      
}

static void restore_descriptors(int saved_in, int saved_out)
{
    if (saved_in >= 0) 
    {
        dup2(saved_in, STDIN_FILENO);
        close(saved_in);
    }

    if (saved_out >= 0)
    {
        dup2(saved_out, STDOUT_FILENO);
        close(saved_out);
    }
}

int handle_builtin(Command *command)
{

    if (!is_builtin(command))
    {
        return 0;
    }
    
    int saved_in = dup(STDIN_FILENO);
    int saved_out = dup(STDOUT_FILENO);

    if (saved_in < 0 || saved_out < 0)
    {
        perror("dup");

        if (saved_in >= 0)
        {
            close(saved_in);
        }

        if (saved_out >= 0)
        {
            close(saved_out);
        }

        return 1;
    }

    if (apply_redirection(command) < 0)
    {   
        fflush(stdout);
        restore_descriptors(saved_in, saved_out);
        return 1;
    }

    if (strcmp(command->argv[0], "cd") == 0)
    {

        if (command->argc < 2)
        {
            printf("Invalid usage of cd: Missing arguments\n");
        }

        else if (chdir(command->argv[1]) != 0)
        {
            perror("cd");
        }
    }

    // pwd
    else if (strcmp(command->argv[0], "pwd") == 0)
    {

        char cwd[1024];

        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            perror("pwd");
        }

        else
        {
            printf("%s\n", cwd);
        }
    }

    // help
    else if (strcmp(command->argv[0], "help") == 0)
    {
        printf(" =-=-=-=-=-=-= ixsh help =-=-=-=-=-=-=-=\n\n");
        printf("cd:               Change directory\n");
        printf("pwd:              Print current directory\n");
        printf("help:             View this manual\n");
        printf("exit:             Exit ixsh\n");

    }

    fflush(stdout);
    restore_descriptors(saved_in, saved_out);

    return 1;
}