#include "parser.h"
#include <stdlib.h>
#include <string.h>

Command parse_command(char *buffer) {

    Command command;
    int capacity = 10;

    command.argc = 0;
    command.input_file = NULL;
    command.output_file = NULL;
    command.append = 0;

    command.argv = malloc(capacity * sizeof(char *));

    if(command.argv == NULL) {
        return command;
    }

    char *token = strtok(buffer, " \t");

    while(token != NULL) {

        // input redirection
        if(strcmp(token, "<") == 0) {

            token = strtok(NULL, " \t");
            
            if (token != NULL) {
                command.input_file = token;
                command.append = 0;
            }
        }

        // output redirection
        else if (strcmp(token, ">") == 0) {

            token = strtok(token, " \t");

            if (token != NULL) {
                command.output_file = token;
                command.append = 0;
            }
        }

        // output redirection (append mode)
        else if (strcmp(token, ">>") == 0) {

            token = strtok(token, " \t");

            if (token != NULL) {
                command.output_file = token;
                command.append = 1;
            }
        }

        else {

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
        }
        
        token = strtok(NULL, " \t");
    }

    command.argv[command.argc] = NULL;

    return command;

}