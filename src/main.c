    #include <stdio.h>
    #include <string.h>
    #include <stdbool.h>
    #include <stdlib.h>

    #include "parser.h"

    int main(void)
    {
        printf("Welcome to ixsh!\n");

        while(true) {

            char buffer[1024];
            printf("ixsh> ");
            fflush(stdout);

            // input like CTRL+Z
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                printf("couldn't handle input\n");
                break;
            }

            // remove newline
            buffer[strcspn(buffer, "\n")] = '\0';

            // break the terminal loop
            if(strcmp(buffer, "exit") == 0) {
                break;
            }
            
            Command command = parse_command(buffer);

            if(command.argv == NULL) {
                printf("Failed to parse the command! \n");
                continue;
            }

            printf("argc: %d\n", command.argc);

            for(int i = 0; i < command.argc; i++) {

                printf("argv[%d] = \"%s\"\n", i, command.argv[i]);
            }

            free(command.argv);
        }

        return 0;
    }