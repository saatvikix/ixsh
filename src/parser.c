#include "parser.h"
#include <stdlib.h>
#include <string.h>

Command parse_command(char *buffer) {

    Command command;
    int capacity = 10;

    command.argc = 0;
    command.argv = malloc(capacity * sizeof(char *));

    if(command.argv == NULL) {
        return command;
    }

    char *token = strtok(buffer, " \t");

    while(token != NULL) {

        if(command.argc >= capacity - 1) {
            capacity *= 2;

            char **temp = realloc(command.argv, capacity * sizeof(char *));

            if(temp == NULL) {
                free(command.argv);
                command.argc = 0;
                command.argv = NULL;
                return command;
            }

            command.argv = temp;
        }

        command.argv[command.argc] = token;
        command.argc++;

        token = strtok(NULL, " \t");
    }

    command.argv[command.argc] = NULL;

    return command;

}